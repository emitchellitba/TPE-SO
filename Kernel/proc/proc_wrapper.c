// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

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

  call_timer_tick();
  while (1) // No se deberia llegar a este punto pues un call_timer_tick debe
            // ocurrir antes
  {
  }
}
