#include <init.h>

#define SHELL_PROGRAM_NAME "shell"

int run_shell(int argc, char *argv[], int *pid) {
  *pid = spawn_process(SHELL_PROGRAM_NAME, argc, argv);

  if (*pid < 0) {
    // Log the error if needed
    return -1;
  }
  // Log success
  return 0;
}

int init_main(int argc, char **argv) {
  load_program(SHELL_PROGRAM_NAME, (uint64_t)&shell_main);

  char *shell_argv_data[] = {SHELL_PROGRAM_NAME, NULL};
  int shell_argc = 1;
  int shell_pid = -1;

  while (1) {
    int err = 0;
    if ((err = run_shell(shell_argc, shell_argv_data, &shell_pid))) {
      break;
    }

    while (1) {
      // int64_t pid = waitpid(-1, NULL, 0); // Espera a cualquier proceso
      // if (pid == shell_pid)
      // {
      //   break;
      // }

      /* Aca hay que limpiar el estado del proceso huerfano que
         termino */
    }
  }

  // Se llega aca si hubo un error al crear el shell
  while (1)
    ;
}
