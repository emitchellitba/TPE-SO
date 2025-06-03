#include <core.h>

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

int test_cmd(void) {
  int pid;
  char *argv[] = {"test", NULL};
  int argc = 1;

  pid = spawn_process("test", argc, argv);
  if (pid < 0) {
    printf("Error spawning process 'test'\n");
    return -1;
  }

  return 0;
}
