#ifndef LIBU_H
#define LIBU_H

#include <lib/wrappers.h>
#include <proc_info.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define READ_PIPE 0
#define WRITE_PIPE 1

#define SPAWN_USE_DEFAULT_IO 0x1
#define SPAWN_INHERIT_FDS 0x2

typedef int (*main_func_t)(int argc, char *argv[]);

typedef int (*shell_command)(int params_count, char *params[]);

DECLARE_WRAPPER(read, (int fd, char *buffer, size_t count))
DECLARE_WRAPPER(write, (int fd, const char *buffer, size_t count))
DECLARE_WRAPPER(sleep_time, (uint64_t time))
DECLARE_WRAPPER(usleep_time, (uint64_t time))
DECLARE_WRAPPER(read_kmsg, (char *log, size_t size))
DECLARE_WRAPPER(pipe_create, (char *id))
DECLARE_WRAPPER(pipe_open, (char *id, int mode))
DECLARE_WRAPPER(pipe_close, (char *id))
DECLARE_WRAPPER(get_procs, (proc_info_t * procs, size_t size))
DECLARE_WRAPPER(load_program, (char *name, int entry))
DECLARE_WRAPPER(rm_program, (char *name))
DECLARE_WRAPPER(get_programs, (char **buffer, int max_count))
DECLARE_WRAPPER(spawn_process,
                (const char *name, int argc, char **argv, int *fds))
DECLARE_WRAPPER(kill_proc, (int pid))
DECLARE_WRAPPER(change_priority, (int pid, int new_priority))
DECLARE_WRAPPER(proc_exit, (int code))
DECLARE_WRAPPER(copy_fd, (int pid, int target_fd, int src_fd))
DECLARE_WRAPPER(close_fd, (int fd))
DECLARE_WRAPPER(wait_pid, (int pid, int *status))
DECLARE_WRAPPER(wait, (int *status))
DECLARE_WRAPPER(get_pid, (void))
DECLARE_WRAPPER(yield, (void))

#endif // LIBU_H
