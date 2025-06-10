#include "libu.h"
#include "test_util.h"
#include <stdint.h>
#include <stdio.h>

#define SEM_ID 20
#define TOTAL_PAIR_PROCESSES 2

int64_t global;

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  yield();
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 3) {
    printf("my_process_inc: Error - Expected 3 arguments, got %d\n", argc);
    return -1;
  }

  if ((n = satoi(argv[0])) <= 0) {
    printf("my_process_inc: Error - Invalid n_iterations value: %s\n", argv[0]);
    return -1;
  }
  inc = satoi(argv[1]);
  if (inc != 1 && inc != -1) {
    printf("my_process_inc: Error - Invalid inc value (must be 1 or -1): %s\n",
           argv[1]);
    return -1;
  }
  if ((use_sem = satoi(argv[2])) < 0 || use_sem > 1) {
    printf(
        "my_process_inc: Error - Invalid use_sem value (must be 0 or 1): %s\n",
        argv[2]);
    return -1;
  }

  semaphore_t *sem;
  if (use_sem) {
    sem = my_sem_open(SEM_ID);
    if (!sem) {
      printf("my_process_inc: ERROR opening semaphore %d\n", SEM_ID);
      return -1;
    }
  }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      my_sem_wait(sem);
    slowInc(&global, inc);
    if (use_sem)
      my_sem_post(sem);
    return 0; // Exit after incrementing/decrementing
  }
  if (use_sem) {
    my_sem_close(sem);
  }
  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) {
  int status;

  if (load_program("my_process_inc", (uint64_t)&my_process_inc) < 0) {
    printf("test_sync: Error loading program my_process_inc\n");
    return -1;
  }

  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  const char *n_iterations_str = "5";
  const char *use_sem_str = "1";

  int use_sem_flag = satoi(use_sem_str);

  char *argvDec[] = {(char *)n_iterations_str, "-1", (char *)use_sem_str, NULL};
  char *argvInc[] = {(char *)n_iterations_str, "1", (char *)use_sem_str, NULL};

  global = 0;
  printf("test_sync: Initial global value: %d\n", global);
  printf("test_sync: Running with N_ITERATIONS = %s, USE_SEMAPHORES = %s\n",
         n_iterations_str, use_sem_str);

  if (use_sem_flag) {
    if (!my_sem_create(SEM_ID, 1)) {
      printf("test_sync: ERROR creating semaphore %d\n", SEM_ID);
      rm_program("my_process_inc");
      return -1;
    }
    printf("test_sync: Semaphore %d created with initial value 1.\n", SEM_ID);
  }

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = spawn_process("my_process_inc", 3, argvDec, NULL);
    if (pids[i] == 0 || pids[i] == (uint64_t)-1) {
      printf("test_sync: Error spawning decrementer process %d\n", i);
    }
    pids[i + TOTAL_PAIR_PROCESSES] =
        spawn_process("my_process_inc", 3, argvInc, NULL);
    if (pids[i + TOTAL_PAIR_PROCESSES] == 0 ||
        pids[i + TOTAL_PAIR_PROCESSES] == (uint64_t)-1) {
      printf("test_sync: Error spawning incrementer process %d\n", i);
    }
  }

  printf("test_sync: All %d processes spawned. Waiting for them to finish...\n",
         2 * TOTAL_PAIR_PROCESSES);

  for (i = 0; i < 2 * TOTAL_PAIR_PROCESSES; i++) {
    if (pids[i] != 0 && pids[i] != (uint64_t)-1) {
      wait_pid(pids[i], &status);
    }
  }

  printf("test_sync: All processes finished.\n");

  if (use_sem_flag) {

    // Destruir el semáforo después de que todos los procesos lo hayan usado y
    // cerrado Asumiendo que tienes una función my_sem_destroy o similar. Si no,
    // el semáforo podría quedar "colgado" en el sistema. El nombre exacto de la
    // función dependerá de tu API de semáforos. Por ejemplo:
    // my_sem_destroy(SEM_ID); Si tu sistema no tiene un "destroy" explícito y
    // los semáforos se limpian de otra forma (ej. al reiniciar), puedes omitir
    // esto, pero es buena práctica tenerlo. Por ahora, lo comentaré si no
    // tienes la función: my_sem_destroy(SEM_ID);
    printf("test_sync: Semaphore %d would be destroyed here if my_sem_destroy "
           "exists.\n",
           SEM_ID);
  }

  // Eliminar el programa 'my_process_inc' del registro de programas.
  rm_program("my_process_inc");

  printf("Final value: %d\n", global);

  // El valor esperado de 'global' es 0 si use_sem_flag es 1.
  // Si use_sem_flag es 0, el valor será probablemente distinto de 0.
  if (use_sem_flag && global != 0) {
    printf("TEST FAILED: Expected global = 0 with semaphores, but got %d\n",
           global);
    return -1; // Indicar fallo
  } else if (!use_sem_flag && global == 0) {
    // Esto es menos probable pero podría ocurrir por casualidad si N es muy
    // bajo.
    printf("TEST WARNING: Expected global != 0 without semaphores, but got 0. "
           "Try with more iterations.\n");
  } else if (use_sem_flag && global == 0) {
    printf("TEST PASSED: global = 0 with semaphores.\n");
  }

  return 0; // Indicar éxito general del test (o el estado específico)
}