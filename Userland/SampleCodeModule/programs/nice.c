// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <libu.h>
#include <stdLibrary.h>
#include <sys/types.h>

int nice_main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s <pid> <new_priority>\n", argv[0]);
    return -1;
  }

  pid_t pid = atoi(argv[1]);
  int new_priority = atoi(argv[2]);

  if (new_priority <= 0 || new_priority > QUANTUM_MAX) {
    printf("Invalid priority value. Must be between 1 and %d.\n", QUANTUM_MAX);
    return -1;
  }

  int res = change_priority(pid, new_priority);
  if (res < 0) {
    printf("Failed to change priority for PID %d\n", pid);
    return -1;
  }

  printf("Priority of PID %d changed to %d\n", pid, new_priority);
  return 0;
}
