#include <core.h>

int exit_cmd() {
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
