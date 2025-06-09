// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <init.h>

extern int ps_main(int argc, char *argv[]);
extern int ls_main(int argc, char *argv[]);
extern int cat_main(int argc, char *argv[]);
extern int lazy_main(void);
extern int sleep_main(int argc, char *argv[]);
extern int loop_main(int argc, char *argv[]);
extern int nice_main(int argc, char *argv[]);
extern int block_main(int argc, char *argv[]);
extern int phylo_main(int argc, char *argv[]);
extern int wc_main(int argc, char *argv[]);
extern int filter_main(int argc, char *argv[]);

extern int rd_wr_test_main(int argc, char *argv[]);
extern int pipes_test_main(int argc, char *argv[]);
extern int sched_test_main(int argc, char *argv[]);
extern int spawn_test_main(void);
extern int test_mm(uint64_t argc, char *argv[]);
extern void test_prio(void);
extern int test_processes(uint64_t argc, char *argv[]);
extern int sync_test_main(int argc, char *argv[]);
extern int test_runner_cmd(int argc, char **argv);

#define SHELL_PROGRAM_NAME "shell"

static int run_shell(int argc, char *argv[], int *pid) {
  *pid = spawn_process(SHELL_PROGRAM_NAME, argc, argv, NULL);

  if (*pid < 0) {
    return -1;
  }
  return 0;
}

static void load_programs() {
  load_program(SHELL_PROGRAM_NAME, (uint64_t)&shell_main);
  load_program("ps", (uint64_t)&ps_main);
  load_program("ls", (uint64_t)&ls_main);
  load_program("cat", (uint64_t)&cat_main);
  load_program("lazy", (uint64_t)&lazy_main);
  load_program("sleep", (uint64_t)&sleep_main);
  load_program("loop", (uint64_t)&loop_main);
  load_program("chprio", (uint64_t)&nice_main);
  load_program("block", (uint64_t)&block_main);
  load_program("test_runner_cmd", (uint64_t)&test_runner_cmd);
  load_program("phylo", (uint64_t)&phylo_main);
  load_program("wc", (uint64_t)&wc_main);
  load_program("filter", (uint64_t)&filter_main);

  load_program("pipes_test", (uint64_t)&pipes_test_main);
  load_program("rd_wr_test", (uint64_t)&rd_wr_test_main);
  load_program("sched_test", (uint64_t)&sched_test_main);
  load_program("spawn_test", (uint64_t)&spawn_test_main);
  load_program("test_mm", (uint64_t)&test_mm);
  load_program("test_prio", (uint64_t)&test_prio);
  load_program("test_processes", (uint64_t)&test_processes);
  load_program("test_sync", (uint64_t)&sync_test_main);
}

int init_main(int argc, char **argv) {
  load_programs();

  char *shell_argv_data[] = {SHELL_PROGRAM_NAME, NULL};
  int shell_argc = 1;
  int shell_pid = -1;

  while (1) {
    int err = 0;
    if ((err = run_shell(shell_argc, shell_argv_data, &shell_pid))) {
      break;
    }

    int pid, status;
    while ((pid = wait(&status)) > 0) {
      if (pid == shell_pid) {
        printf("\n\nShell process died!\n");
        printf("Restarting shell...\n");
        break;
      }
    }
  }

  // Se llega aca si hubo un error al crear el shell
  while (1)
    ;
}
