#include <libu.h>
#include <stdLibrary.h>

int sched_test_main(int argc, char *argv[]);
int sleeping_process_main(void);

/**
 * Este test va a crear muchos procesos hijos y los va a esperar
 * a todos con wait para ver si se recolectan correctamente.
 * Los hijos imprimiran un texto varias veces para probar
 * que el scheduler los intercala correctamente
 */
int sched_test_main(int argc, char *argv[]) {
  load_program("sleeping_program", sleeping_process_main);

  for (int i = 0; i < 10; i++) {
    int pid = spawn_process("sleeping_program", 0, NULL, NULL);
    if (pid < 0) {
      write(STDERR, "Error spawning process\n", 24);
      return 1;
    }
  }

  int status;
  while (wait(&status) > 0)
    ;

  rm_program("sleeping_program");

  return 0;
}

int sleeping_process_main(void) {
  int pid = get_pid();
  for (int i = 0; i < 2; i++) {
    printf("Process %d started, sleeping for 5 seconds...\n", pid);
    sleep_time(100);
  }
  return 0;
}
