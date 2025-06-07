#include <init.h>

extern int ps_main(int argc, char *argv[]);
extern int ls_main(int argc, char *argv[]);
extern int rd_wr_test_main(int argc, char *argv[]);
extern int pipes_test_main(int argc, char *argv[]);
extern int cat_main(int argc, char *argv[]);

#define SHELL_PROGRAM_NAME "shell"

static int run_shell(int argc, char *argv[], int *pid) {
  *pid = spawn_process(SHELL_PROGRAM_NAME, argc, argv);

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
  load_program("test", (uint64_t)&pipes_test_main);
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
      if (pid == shell_pid)
        break;
    }
  }

  // Se llega aca si hubo un error al crear el shell
  while (1)
    ;
}
