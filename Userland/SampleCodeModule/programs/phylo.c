// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <libu.h>
#include <stdLibrary.h>

#define MAX_PHILOSOPHERS 10
#define MIN_PHILOSOPHERS 2

typedef enum { THINKING, HUNGRY, EATING } state_t;
const char *state_str[] = {"Thinking", "Hungry", "Eating"};

typedef struct {
  int pid;
  int pipe_fd;
  char pipe_id[16];
  state_t state;
} philosopher_t;

philosopher_t philosophers[MAX_PHILOSOPHERS];
int n_philosophers = 5;
semaphore_t *fork_mutex[MAX_PHILOSOPHERS];

// Sends the current state to the parent process through the pipe
void send_state(int pipe_fd, state_t state) {
  char msg = (char)state;
  write(pipe_fd, &msg, 1);
}

// Philosopher process logic with fork mutexes
void philosopher_loop(int id, char *pipe_id, int n_forks) {
  int pipe_fd = pipe_open(pipe_id, WRITE_PIPE);
  int left = id;
  int right = (id + 1) % n_forks;

  semaphore_t *left_mutex = my_sem_open(left);
  semaphore_t *right_mutex = my_sem_open(right);

  state_t state;

  while (1) {
    state = THINKING;
    send_state(pipe_fd, state);
    sleep_time(1 + (get_random() % 2));

    state = HUNGRY;
    send_state(pipe_fd, state);

    if (id % 2 == 0) {
      my_sem_wait(left_mutex);
      my_sem_wait(right_mutex);
    } else {
      my_sem_wait(right_mutex);
      my_sem_wait(left_mutex);
    }

    state = EATING;
    send_state(pipe_fd, state);
    sleep_time(2);

    my_sem_post(left_mutex);
    my_sem_post(right_mutex);
  }
}

// Input process: reads STDIN and signals master via semaphores
void phylo_input_main() {
  semaphore_t *add_sem = my_sem_open(1000);    // Arbitrary id for "add"
  semaphore_t *remove_sem = my_sem_open(1001); // Arbitrary id for "remove"
  while (1) {
    char c = 0;
    if (read(STDIN, &c, 1) == 1) {
      if (c == 'a')
        my_sem_post(add_sem);
      if (c == 'r')
        my_sem_post(remove_sem);
    }
    sleep_time(1);
  }
}

// Prints the current table state
void print_table() {
  for (int i = 0; i < n_philosophers; i++) {
    printf("F%d: %-8s ", i, state_str[philosophers[i].state]);
  }
  printf("\n");
}

// Adds a new philosopher process and fork mutex
void add_philosopher() {
  if (n_philosophers >= MAX_PHILOSOPHERS)
    return;

  fork_mutex[n_philosophers] = my_sem_create(n_philosophers, 1);

  str_ncpy(philosophers[n_philosophers].pipe_id, "phylo", 6);
  char num[4];
  itoa(n_philosophers, num);
  str_ncpy(philosophers[n_philosophers].pipe_id + 5, num, 4);

  pipe_create(philosophers[n_philosophers].pipe_id);
  philosophers[n_philosophers].pipe_fd =
      pipe_open(philosophers[n_philosophers].pipe_id, READ_PIPE);

  char n_forks_str[4];
  itoa(n_philosophers + 1, n_forks_str);
  char *argv[4];
  argv[0] = "phylo";
  argv[1] = philosophers[n_philosophers].pipe_id;
  argv[2] = n_forks_str;
  argv[3] = NULL;

  int pid = spawn_process("phylo", 3, argv, NULL);
  philosophers[n_philosophers].pid = pid;
  philosophers[n_philosophers].state = THINKING;
  n_philosophers++;
}

// Removes the last philosopher process and fork mutex
void remove_philosopher() {
  if (n_philosophers <= MIN_PHILOSOPHERS)
    return;
  n_philosophers--;
  kill_proc(philosophers[n_philosophers].pid);
  pipe_close(philosophers[n_philosophers].pipe_id);
  my_sem_close(fork_mutex[n_philosophers]);
}

// Reads the state from each philosopher's pipe
void update_states() {
  char msg;
  for (int i = 0; i < n_philosophers; i++) {
    while (read(philosophers[i].pipe_fd, &msg, 1) == 1) {
      philosophers[i].state = (state_t)msg;
    }
  }
}

// Master entry point
int phylo_main(int argc, char *argv[]) {
  // If called as a philosopher process
  if (argc == 3) {
    int id = atoi(argv[1] + 5); // "phyloX" -> X
    int n_forks = atoi(argv[2]);
    philosopher_loop(id, argv[1], n_forks);
    return 0;
  }
  // If called as input process
  if (argc == 2 && str_cmp(argv[1], "input") == 0) {
    phylo_input_main();
    return 0;
  }

  // Parent process: create initial fork mutexes and philosophers
  for (int i = 0; i < n_philosophers; i++) {
    fork_mutex[i] = my_sem_create(i, 1);
  }

  for (int i = 0; i < n_philosophers; i++) {
    str_ncpy(philosophers[i].pipe_id, "phylo", 6);
    char num[4];
    itoa(i, num);
    str_ncpy(philosophers[i].pipe_id + 5, num, 4);

    pipe_create(philosophers[i].pipe_id);
    philosophers[i].pipe_fd = pipe_open(philosophers[i].pipe_id, READ_PIPE);

    char n_forks_str[4];
    itoa(n_philosophers, n_forks_str);
    char *argv_child[4];
    argv_child[0] = "phylo";
    argv_child[1] = philosophers[i].pipe_id;
    argv_child[2] = n_forks_str;
    argv_child[3] = NULL;

    int pid = spawn_process("phylo", 3, argv_child, NULL);
    philosophers[i].pid = pid;
    philosophers[i].state = THINKING;
  }

  // Create semaphores for input communication
  my_sem_create(1000, 0); // add
  my_sem_create(1001, 0); // remove

  // Launch input process
  char *argv_input[3] = {"phylo", "input", NULL};
  spawn_process("phylo", 2, argv_input, NULL);

  print_table();

  // Main loop: update states and handle input via semaphores
  semaphore_t *add_sem = my_sem_open(1000);
  semaphore_t *remove_sem = my_sem_open(1001);

  while (1) {
    update_states();
    print_table();

    // Wait for input process to signal add/remove
    if (my_sem_trywait(add_sem) == 0) {
      add_philosopher();
    }
    if (my_sem_trywait(remove_sem) == 0) {
      remove_philosopher();
    }

    sleep_time(1);
  }

  return 0;
}
