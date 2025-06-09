#include <libu.h>
#include <stdLibrary.h>

int spawn_test_main(void);

/**
 * Este test busca instanciar una gran cantidad de procesos y esperarlos
 * multiples veces para verificar que el sistema de spawn y wait funciona
 * correctamente.
 */
int spawn_test_main(void) {
  int i;
  int pid;
  int status;

  for (int j = 0; j < 3; j++) {
    for (i = 0; i < 10; i++) {
      pid = spawn_process("sleep", 2, (char *[]){"sleep", "5", NULL}, NULL);
      if (pid < 0) {
        printf("Error spawning process %d\n", i);
        return -1;
      }

      printf("Process %d spawned with status %d\n", pid, status);
    }

    while ((pid = wait(&status)) > 0) {
      printf("Process %d finished with status %d\n", pid, status);
    }
  }

  return 0;
}
