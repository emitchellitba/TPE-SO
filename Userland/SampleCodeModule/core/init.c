#include <init.h>
#include <libu.h>
#include <shell.h>
#include <stdLibrary.h>
#include <stdint.h>

int run_shell(char *name, int argc, char *argv[], int *pid) {
  *pid = new_proc(name, (uint64_t)&shell_main, argc, argv);

  if (*pid < 0) {
    // Log the error if needed
    return -1;
  }
  // Log success
  return 0;
}

int init_main(int argc, char **argv) {
  char *shell_argv_data[] = {"shell", NULL};
  int shell_argc = 1;
  int shell_pid = -1;

  while (1) {
    int err = 0;
    if ((err = run_shell("shell", shell_argc, shell_argv_data, &shell_pid))) {
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
