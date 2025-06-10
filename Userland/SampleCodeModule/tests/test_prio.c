#include "libu.h"
#include <stdint.h>
#include <stdio.h>
// #include "syscall.h"
#include "test_util.h"

#define MINOR_WAIT                                                             \
  1000000 // TODO: Change this value to prevent a process from flooding the
          // screen
#define WAIT                                                                   \
  10000000 // TODO: Change this value to make the wait long enough to see theese
           // processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0  // TODO: Change as required
#define MEDIUM 1  // TODO: Change as required
#define HIGHEST 2 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  int pid;
  int status;

  char *argv[] = {
      "endless_loop_print",
      "10", // Argument to endless_loop_print, can be adjusted as needed
      NULL  // End of arguments
  };
  uint64_t i;

  load_program("endless_loop_print", (uint64_t)&endless_loop_print);

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    pids[i] = spawn_process("endless_loop_print", 2, argv, NULL);
  }

  sleep_time(5);
  printf("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    change_priority(pids[i], prio[i]);

  sleep_time(5);
  printf("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    block_proc(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    change_priority(pids[i], MEDIUM);

  printf("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    unblock_proc(pids[i]);

  sleep_time(5);
  printf("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    printf("Killing process with PID: %d\n", pids[i]);
    kill_proc(pids[i]);
  }

  while ((pid = wait(&status)) > 0) {
    printf("Process %d finished with status %d\n", pid, status);
  }

  rm_program("endless_loop_print");
  printf("DONE!\n");
}