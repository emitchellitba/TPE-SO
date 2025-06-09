#include "syscall.h"
#include "test_util.h"
#include <libu.h>
#include <stdint.h>
#include <stdio.h>

#define SEM_ID 0
#define TOTAL_PAIR_PROCESSES 2

int64_t global;
semaphore_t *sem;

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

  if (argc != 3)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

  if (use_sem)
    if (!my_sem_open(SEM_ID)) {
      printf("test_sync: ERROR opening semaphore\n");
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      my_sem_wait(SEM_ID);
    slowInc(&global, inc);
    if (use_sem)
      my_sem_post(SEM_ID);
  }

  if (use_sem)
    my_sem_close(SEM_ID);

  return 0;
}

int sync_test_main(int argc, char *argv[]) { //{n, use_sem, 0}
  load_program("my_process_inc", (uint64_t)&my_process_inc);

  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 2)
    return -1;

  char *argvDec[] = {argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {argv[0], "1", argv[1], NULL};

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = spawn_process_bg("my_process_inc", 3, argvDec, NULL);
    pids[i + TOTAL_PAIR_PROCESSES] =
        spawn_process_bg("my_process_inc", 3, argvInc, NULL);
  }

  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    wait_pid(pids[i], NULL);
    wait_pid(pids[i + TOTAL_PAIR_PROCESSES], NULL);
  }

  printf("Final value: %d\n", global);

  rm_program("my_process_inc");
  return 0;
}
