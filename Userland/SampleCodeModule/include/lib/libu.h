#ifndef LIBU_H
#define LIBU_H

#include <lib/wrappers.h>

#define STDIN 0
#define STDOUT 1

typedef int (*main_func_t)(int argc, char *argv[]);

typedef int (*shell_command)(int params_count, char *params[]);

typedef struct command_entry {
  const char *name;
  shell_command func;
} command_entry_t;

typedef struct {
  int pid;
  int ppid;
  char name[32];
  int state;
  int priority;
} proc_info_t;

DECLARE_WRAPPER(read, (int fd, char *buffer, size_t count))
DECLARE_WRAPPER(write, (int fd, const char *buffer, size_t count))
DECLARE_WRAPPER(read_kmsg, (char *log, size_t size))
DECLARE_WRAPPER(pipe_create, (char *id))
DECLARE_WRAPPER(pipe_open, (char *id, int mode))
DECLARE_WRAPPER(pipe_close, (char *id))
DECLARE_WRAPPER(get_procs, (proc_info_t * procs, size_t size))
DECLARE_WRAPPER(load_program, (char *name, int entry))
DECLARE_WRAPPER(rm_program, (char *name))
DECLARE_WRAPPER(get_programs, (char **buffer, int max_count))
DECLARE_WRAPPER(spawn_process, (char *name, int argc, char **argv))
DECLARE_WRAPPER(kill_proc, (int pid))
DECLARE_WRAPPER(change_priority, (int pid, int new_priority))
DECLARE_WRAPPER(proc_exit, (int code))
DECLARE_WRAPPER(dup2_fd, (int oldfd, int newfd))
DECLARE_WRAPPER(close_fd, (int fd))

#endif // LIBU_H
