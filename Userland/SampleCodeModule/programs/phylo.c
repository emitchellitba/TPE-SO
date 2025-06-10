// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <libu.h>

#define MAX_PHY 10
#define MIN_PHY 3
#define NO_PID -1
#define SEM_ID 100
#define SEM_ADD 200
#define SEM_REMOVE 201
#define SEM_QUIT 202
#define ADD_VALUE 'a'
#define REMOVE_VALUE 'r'
#define QUIT_VALUE 'q'
#define SEM_PHYLO(x) (SEM_ID + (x) + 1)
#define RIGHT_PHYLO(x) (((x) + 1) % phylo_count)
#define LEFT_PHYLO(x) (((x) + phylo_count - 1) % phylo_count)

typedef enum { NONE = 0, DRIVING, WAITING, THINKING } PHYLO_STATE;

static char state_chars[] = {'0', 'E', '.', '.'};

// Cambia los nombres por celebridades argentinas
static char *phylo_names[] = {
    "Messi",   "Mirtha", "Colapinto", "Borges",        "Ricky Fort",
    "Tinelli", "Coscu",  "Del Potro", "Dibu Martinez", "Mauro Icardi"};

static PHYLO_STATE phylo_states[MAX_PHY];
static int16_t phylo_pids[MAX_PHY];
static uint8_t phylo_count = 0;

void phylo_input_process();

void print_status() {
  uint8_t write = 0;
  for (int i = 0; i < phylo_count; i++) {
    if (phylo_states[i] != NONE) {
      write = 1;
      printf("%c ", state_chars[phylo_states[i]]);
    }
  }
  if (write) {
    printf("\n");
  }
}

void leave_pedals(uint8_t idx) {
  my_sem_wait(SEM_ID);
  phylo_states[idx] = THINKING;
  if (phylo_states[LEFT_PHYLO(idx)] == WAITING &&
      phylo_states[LEFT_PHYLO(LEFT_PHYLO(idx))] != DRIVING) {
    phylo_states[LEFT_PHYLO(idx)] = DRIVING;
    my_sem_post(SEM_PHYLO(LEFT_PHYLO(idx)));
    print_status();
  }

  if (phylo_states[RIGHT_PHYLO(idx)] == WAITING &&
      phylo_states[RIGHT_PHYLO(RIGHT_PHYLO(idx))] != DRIVING) {
    phylo_states[RIGHT_PHYLO(idx)] = DRIVING;
    my_sem_post(SEM_PHYLO(RIGHT_PHYLO(idx)));
    print_status();
  }
  my_sem_post(SEM_ID);
}

void take_pedals(uint8_t idx) {
  my_sem_wait(SEM_ID);
  phylo_states[idx] = WAITING;
  if (phylo_states[LEFT_PHYLO(idx)] != DRIVING &&
      phylo_states[RIGHT_PHYLO(idx)] != DRIVING) {
    phylo_states[idx] = DRIVING;
    my_sem_post(SEM_PHYLO(idx));
    print_status();
  }
  my_sem_post(SEM_ID);
  my_sem_wait(SEM_PHYLO(idx));
}

void phylo_process(uint8_t argc, char *argv[]) {
  uint8_t index_phylo = satoi(argv[0]);
  m_free(argv[0]);
  m_free(argv);
  printf("%s se sienta a la mesa del asado.\n", phylo_names[index_phylo]);
  phylo_states[index_phylo] = THINKING;
  while (1) {
    sleep_time(2);
    take_pedals(index_phylo);
    sleep_time(2);
    leave_pedals(index_phylo);
  }
}

int8_t new_phylo(uint8_t idx) {
  load_program(phylo_names[idx], (uint64_t)&phylo_process);

  my_sem_wait(SEM_ID);
  if (my_sem_create(SEM_PHYLO(idx), 0) == -1) {
    return -1;
  }

  char **argv = m_malloc(sizeof(char *) * 2);
  argv[0] = m_malloc(10);
  itos(idx, argv[0]);
  argv[1] = NULL;
  phylo_pids[idx] = spawn_process_bg(phylo_names[idx], 1, argv, NULL);

  if (phylo_pids[idx] != NO_PID) {
    phylo_count++;
  }
  print_status();
  my_sem_post(SEM_ID);
  return phylo_pids[idx] != NO_PID ? 0 : -1;
}

void remove_phylo(uint8_t idx) {
  my_sem_wait(SEM_ID);
  printf("%s se va de la mesa del asado.\n", phylo_names[idx]);
  while (phylo_states[LEFT_PHYLO(idx)] == DRIVING &&
         phylo_states[RIGHT_PHYLO(idx)] == DRIVING) {
    my_sem_post(SEM_ID);
    my_sem_wait(SEM_PHYLO(idx));
    my_sem_wait(SEM_ID);
  }
  kill_proc(phylo_pids[idx]);
  wait_pid(phylo_pids[idx], 1);
  my_sem_close(SEM_PHYLO(idx));
  phylo_pids[idx] = NO_PID;
  phylo_states[idx] = NONE;
  phylo_count--;
  my_sem_post(SEM_ID);

  rm_program(phylo_names[idx]);

  return;
}

void remove_all(int max) {
  for (int i = 0; i < max; i++) {
    remove_phylo(i);
  }
}

int8_t phylo_main() {
  printf("Problema de los filosofos... pero con celebridades argentinas "
         "comiendo asado.\nCada celebridad necesita dos cubiertos para poder "
         "comer.\nPresioná 'a' para sumar una celebridad, 'r' para sacar una o "
         "'q' para terminar.\n");

  semaphore_t *sem = NULL;

  if (my_sem_create(SEM_ID, 1)) {
    printf("No se pudo iniciar la mesa del asado.\n");
    return -1;
  }

  // Crear semáforos para comunicación si no existen
  my_sem_create(SEM_ADD, 0);
  my_sem_create(SEM_REMOVE, 0);
  my_sem_create(SEM_QUIT, 0);

  for (uint8_t i = 0; i < MAX_PHY; i++) {
    phylo_states[i] = NONE;
    phylo_pids[i] = NO_PID;
  }

  for (int i = 0; i < MIN_PHY; i++) {
    if (new_phylo(i) == -1) {
      printf("No se pudo empezar el asado.\n");
      remove_all(i);
      return -1;
    }
  }

  load_program("phylo_input", (uint64_t)&phylo_input_process);

  // Iniciar el proceso de input
  uint64_t input_pid = spawn_process("phylo_input", 0, NULL, NULL);

  semaphore_t *sem_add = my_sem_open(SEM_ADD);
  semaphore_t *sem_remove = my_sem_open(SEM_REMOVE);
  semaphore_t *sem_quit = my_sem_open(SEM_QUIT);

  int quit = 0;
  while (!quit) {
    // Chequeo no bloqueante para agregar
    if (my_sem_trywait(SEM_ADD) == 0) {
      if (phylo_count < MAX_PHY) {
        if (new_phylo(phylo_count) == -1)
          printf("No se pudo agregar una nueva celebridad.\n");
      } else {
        printf("Se alcanzó el máximo de celebridades en la mesa.\n");
      }
      printf("\n");
    }
    // Chequeo no bloqueante para quitar
    if (my_sem_trywait(SEM_REMOVE) == 0) {
      if (phylo_count > MIN_PHY) {
        remove_phylo(phylo_count - 1);
      } else {
        printf("No puede haber menos celebridades en la mesa.\n");
      }
      printf("\n");
    }
    // Chequeo para terminar
    if (my_sem_trywait(SEM_QUIT) == 0) {
      quit = 1;
      // Terminar todos los hijos antes de salir
      remove_all(phylo_count);
      int status;
      wait_pid(input_pid, &status);
      rm_program("phylo_input");
      my_sem_close(SEM_ID);
      printf("El asado terminó. ¡Gracias por participar!\n");
      return 0;
    }
    // Opcional: sleep para evitar busy waiting
    sleep_time(1);
  }

  return 0;
}

void phylo_input_process() {
  set_canonical_mode(0);
  semaphore_t *sem_add = my_sem_open(SEM_ADD);
  semaphore_t *sem_remove = my_sem_open(SEM_REMOVE);
  semaphore_t *sem_quit = my_sem_open(SEM_QUIT);

  char cmd = 0;
  while (1) {
    cmd = get_char();
    if (cmd == ADD_VALUE) {
      my_sem_post(SEM_ADD);
    } else if (cmd == REMOVE_VALUE) {
      my_sem_post(SEM_REMOVE);
    } else if (cmd == QUIT_VALUE) {
      my_sem_post(SEM_QUIT); // Señal para terminar el programa principal
      break;
    }
  }
  set_canonical_mode(1);
  proc_exit(0);
}
