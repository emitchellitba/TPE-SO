#ifndef LIBU_H
#define LIBU_H

#include <lib/wrappers.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define READ_PIPE 0
#define WRITE_PIPE 1

#define SPAWN_USE_DEFAULT_IO 0x1
#define SPAWN_INHERIT_FDS 0x2

typedef enum proc_state_t {
  DEAD,
  READY,
  RUNNING,
  ZOMBIE,
  BLOCKED
} proc_state_t;

typedef int (*main_func_t)(int argc, char *argv[]);

typedef int (*shell_command)(int params_count, char *params[]);

typedef struct {
  int pid;
  int ppid;
  char name[32];
  int state;
  int priority;
} proc_info_t;

DECLARE_WRAPPER(read, (int fd, char *buffer, size_t count))
DECLARE_WRAPPER(write, (int fd, const char *buffer, size_t count))
DECLARE_WRAPPER(sleep_time, (int time))
DECLARE_WRAPPER(read_kmsg, (char *log, size_t size))
DECLARE_WRAPPER(pipe_create, (char *id))
DECLARE_WRAPPER(pipe_open, (char *id, int mode))
DECLARE_WRAPPER(pipe_close, (char *id))
DECLARE_WRAPPER(get_procs, (proc_info_t * procs, size_t size))
DECLARE_WRAPPER(load_program, (char *name, int entry))
DECLARE_WRAPPER(rm_program, (char *name))
DECLARE_WRAPPER(get_programs, (char **buffer, int max_count))
DECLARE_WRAPPER(spawn_process, (const char *name, int argc, char **argv))
DECLARE_WRAPPER(spawn_process_redirect,
                (const char *name, int argc, char **argv, int redirect,
                 int red_fds[2]))
DECLARE_WRAPPER(kill_proc, (int pid))
DECLARE_WRAPPER(change_priority, (int pid, int new_priority))
DECLARE_WRAPPER(proc_exit, (int code))
DECLARE_WRAPPER(copy_fd, (int pid, int target_fd, int src_fd))
DECLARE_WRAPPER(close_fd, (int fd))
DECLARE_WRAPPER(wait_pid, (int pid, int *status))
DECLARE_WRAPPER(wait, (int *status))
DECLARE_WRAPPER(get_pid, (void))

#endif // LIBU_H
