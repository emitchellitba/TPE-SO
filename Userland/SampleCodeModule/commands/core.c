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

int my_script_cmd(void) {
  int pid;
  char *argv[] = {"my_script", NULL};
  int argc = 1;

  pid = spawn_process("my_script", argc, argv);
  if (pid < 0) {
    printf("Error spawning process 'my_script'\n");
    return -1;
  }

  return 0;
}
