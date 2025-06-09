#include "libu.h"
#include "test_util.h"
#include <stdio.h>

typedef struct P_rq {
  int32_t pid;
  enum proc_state_t state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {
  load_program("endless_loop_print", (uint64_t)&endless_loop_print);
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes = 10;
  char *argvAux[] = {
      "endless_loop_print",
      "5", // Argument to endless_loop_print, can be adjusted as needed
      NULL // End of arguments
  };

  p_rq p_rqs[max_processes];

  while (1) {

    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      p_rqs[rq].pid = spawn_process("endless_loop_print", 2, argvAux, NULL);

      if (p_rqs[rq].pid == -1) {
        printf("test_processes: ERROR creating process\n");
        return -1;
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been
    // killed
    while (alive > 0) {

      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
        case 0:
          if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
            if (kill_proc(p_rqs[rq].pid) == -1) {
              printf("test_processes: ERROR killing process\n");
              return -1;
            }
            p_rqs[rq].state = DEAD;
            alive--;
          }
          break;

        case 1:
          if (p_rqs[rq].state == RUNNING) {
            if (block_proc(p_rqs[rq].pid) == -1) {
              printf("test_processes: ERROR blocking process\n");
              return -1;
            }
            p_rqs[rq].state = BLOCKED;
          }
          break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          if (unblock_proc(p_rqs[rq].pid) == -1) {
            printf("test_processes: ERROR unblocking process\n");
            return -1;
          }
          p_rqs[rq].state = RUNNING;
        }
    }
    // Wait for all processes to finish
    for (rq = 0; rq < max_processes; rq++) {
      int status;
      if (wait_pid(p_rqs[rq].pid, &status) == -1) {
        printf("test_processes: ERROR waiting for process\n");
        return -1;
      }
      if (status != 0) {
        printf("test_processes: Process %d finished with status %d\n",
               p_rqs[rq].pid, status);
      }
    }
    printf("test_processes: All processes finished successfully\n");
    break; // Exit the loop after one complete run
  }

  rm_program("endless_loop_print");
}