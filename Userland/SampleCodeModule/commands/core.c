#include <core.h>

#include <libu.h>
#include <stdLibrary.h>

int exit_cmd(void) {
  printf("\n");
  printf("Saving session...\n");
  printf("Session saved\n");
  printf("Exiting...\n");
  printf("\n");

  printf("\n[Process completed]\n");

  /* Aca se debe llamar a sys_exit para terminar el proceso
  shell */
  return 0;
}

int sleep_cmd(int argc, char **argv) {
  if (argc < 1) {
    printf("Usage: sleep <ticks>\n");
    return -1;
  }

  int ticksToWait = atoi(argv[0]);
  if (ticksToWait <= 0) {
    printf("Error: sleep requires a positive number of ticks to wait.\n");
    return -1;
  }

  printf("Sleeping for %d ticks...\n", ticksToWait);
  sleep_time(ticksToWait);
  printf("Woke up after %d ticks.\n", ticksToWait);

  return 0;
}
