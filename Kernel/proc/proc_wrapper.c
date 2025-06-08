
#include <kernel_asm.h>
#include <process.h>
#include <scheduler.h>
#include <stdint.h>

/**
 * Funcion proveida por el kernel que envuelve a todos los procesos que se
 * ejecutan en el sistema. Esta funcion controla la inicializacion y terminacion
 * de los procesos en caso de que no ejecuten exit.
 */
int process_wrapper(uint64_t user_argc, char **user_argv) {
  proc_t *current_p = get_running();
  int ret_val;

  if (current_p && current_p->entry) {
    ret_val = current_p->entry(user_argc, user_argv);
  } else {
    do_exit(-1);
  }

  do_exit(ret_val);

  while (1) // No se deberia llegar a este punto pues un sys_exit debe ocurrir
            // antes
  {
  }
}
