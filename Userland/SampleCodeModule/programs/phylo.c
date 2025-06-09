#include <libu.h>
#include <stdLibrary.h>

#define MAX_PHILOSOPHERS 10
#define MIN_PHILOSOPHERS 2

typedef enum { THINKING, HUNGRY, EATING } state_t;
const char *state_str[] = { "Thinking", "Hungry", "Eating" };

typedef struct {
    int pid;
    int pipe_fd;
    char pipe_id[16];
    state_t state;
} philosopher_t;

philosopher_t philosophers[MAX_PHILOSOPHERS];
int n_philosophers = 5;

// Fork mutexes (semaphores)
int fork_mutex[MAX_PHILOSOPHERS];
char fork_mutex_name[MAX_PHILOSOPHERS][16];

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

    // Open fork mutexes
    char left_name[16], right_name[16];
    str_ncpy(left_name, "fork", 5);
    itoa(left, left_name + 4);
    str_ncpy(right_name, "fork", 5);
    itoa(right, right_name + 4);

    int left_mutex = sem_open(left_name);
    int right_mutex = sem_open(right_name);

    state_t state;

    while (1) {
        // Thinking
        state = THINKING;
        send_state(pipe_fd, state);
        sleep_time(1 + (get_random() % 2));

        // Hungry
        state = HUNGRY;
        send_state(pipe_fd, state);

        // Asymmetric solution: even pick left then right, odd pick right then left
        if (id % 2 == 0) {
            sem_wait(left_mutex);
            sem_wait(right_mutex);
        } else {
            sem_wait(right_mutex);
            sem_wait(left_mutex);
        }

        // Eating
        state = EATING;
        send_state(pipe_fd, state);
        sleep_time(2);

        // Release forks
        sem_post(left_mutex);
        sem_post(right_mutex);
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

    // Create fork mutex for the new fork (between last and new philosopher)
    str_ncpy(fork_mutex_name[n_philosophers], "fork", 5);
    itoa(n_philosophers, fork_mutex_name[n_philosophers] + 4);
    fork_mutex[n_philosophers] = sem_create(fork_mutex_name[n_philosophers], 1);

    // Prepare pipe for the philosopher
    str_ncpy(philosophers[n_philosophers].pipe_id, "phylo", 6);
    char num[4];
    itoa(n_philosophers, num);
    str_ncpy(philosophers[n_philosophers].pipe_id + 5, num, 4);

    pipe_create(philosophers[n_philosophers].pipe_id);
    philosophers[n_philosophers].pipe_fd = pipe_open(philosophers[n_philosophers].pipe_id, READ_PIPE);

    // Prepare argv for the philosopher process
    char n_forks_str[4];
    itoa(n_philosophers + 1, n_forks_str); // Pass current number of forks
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
    sem_unlink(fork_mutex_name[n_philosophers]);
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

// Entry point for the program
int phylo_main(int argc, char *argv[]) {
    // If called as a philosopher process
    if (argc == 3) {
        // argv[1] is the pipe_id, argv[2] is the number of forks
        int id = atoi(argv[1] + 5); // "phyloX" -> X
        int n_forks = atoi(argv[2]);
        philosopher_loop(id, argv[1], n_forks);
        return 0;
    }

    // Parent process: create initial fork mutexes and philosophers
    for (int i = 0; i < n_philosophers; i++) {
        str_ncpy(fork_mutex_name[i], "fork", 5);
        itoa(i, fork_mutex_name[i] + 4);
        fork_mutex[i] = sem_create(fork_mutex_name[i], 1);
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

    print_table();

    // Main loop: update states and handle input
    while (1) {
        update_states();
        print_table();

        // Non-blocking keyboard read
        char c = 0;
        if (read(STDIN, &c, 1) == 1) {
            if (c == 'a') add_philosopher();
            if (c == 'r') remove_philosopher();
        }

        sleep_time(1);
    }

    return 0;
}