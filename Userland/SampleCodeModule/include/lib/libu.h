#ifndef LIBU_H
#define LIBU_H

#include <lib/wrappers.h>
#include <stdLibrary.h>

typedef int (*main_func_t)(int argc, char *argv[]);

typedef int (*terminal_command)();

typedef struct command_entry {
  const char *name;
  terminal_command func;
} command_entry_t;

typedef struct {
  int pid;
  int ppid;
  char name[32];
  int state;
  int priority;
} proc_info_t;

#endif // LIBU_H
