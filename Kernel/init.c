#include <errno.h>
#include <lib/logger.h>
#include <stdlib.h>

int init_main(int argc, char **argv) {
  printk("Init process started\n");

  /**
   * Crear proceso de shell y el timer se encargara de
   * ejecutarlo
   */
  // sys_new_proc("shell", (proc_main_function)SampleCodeModuleAddress, null);

  while (1)
    ;

  return 0;
}
