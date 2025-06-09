// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <core.h>

#include <libu.h>
#include <stdLibrary.h>

int kill_cmd(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: kill <pid>\n");
    return -1;
  }

  int pid = atoi(argv[1]);
  if (pid <= 1) {
    printf("Error: kill requires a valid process ID (PID).\n");
    return -1;
  }

  return kill_proc(pid);
}

int echo_cmd(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: echo <message>\n");
    return -1;
  }

  for (int i = 1; i < argc; i++) {
    printf("%s ", argv[i]);
  }
  printf("\n");

  return 0;
}
