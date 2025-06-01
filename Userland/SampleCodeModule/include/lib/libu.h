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

#endif // LIBU_H
