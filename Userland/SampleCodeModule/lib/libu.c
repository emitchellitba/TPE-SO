#include <libu.h>

DEFINE_WRAPPER(get_date_time, (const void *dt), (dt))
DEFINE_WRAPPER(screen_clear, (void), ())
DEFINE_WRAPPER(change_color, (uint8_t background), (background))
// DEFINE_WRAPPER(opCodeExc, (void), ())
DEFINE_WRAPPER(sleep_time, (uint64_t time), (time))
DEFINE_WRAPPER(usleep_time, (uint64_t time), (time))
DEFINE_WRAPPER(zoom, (int in), (in))
DEFINE_WRAPPER(get_regist, (uint64_t *regs), (regs))
// DEFINE_WRAPPER(make_sound, (int64_t time, int64_t nFrequence), (time,
// nFrequence))
DEFINE_WRAPPER(read, (int fd, char *buffer, size_t count), (fd, buffer, count))
DEFINE_WRAPPER(write, (int fd, const char *buffer, size_t count),
               (fd, buffer, count))
DEFINE_WRAPPER(read_kmsg, (char *log, size_t size), (log, size))
DEFINE_WRAPPER(pipe_create, (char *id), (id))
DEFINE_WRAPPER(pipe_open, (char *id, int mode), (id, mode))
DEFINE_WRAPPER(pipe_close, (char *id), (id))
DEFINE_WRAPPER(get_procs, (proc_info_t * procs, size_t size), (procs, size))
DEFINE_WRAPPER(load_program, (char *name, int entry), (name, entry))
DEFINE_WRAPPER(rm_program, (char *name), (name))
DEFINE_WRAPPER(get_programs, (char **buffer, int max_count),
               (buffer, max_count))
DEFINE_WRAPPER(kill_proc, (int pid), (pid))
DEFINE_WRAPPER(change_priority, (int pid, int new_priority),
               (pid, new_priority))
DEFINE_WRAPPER(proc_exit, (int code), (code))
DEFINE_WRAPPER(block_proc, (int pid), (pid))
DEFINE_WRAPPER(unblock_proc, (int pid), (pid))
DEFINE_WRAPPER(copy_fd, (int pid, int target_fd, int src_fd),
               (pid, target_fd, src_fd))
DEFINE_WRAPPER(close_fd, (int fd), (fd))
DEFINE_WRAPPER(wait_pid, (int pid, int *status), (pid, status))
DEFINE_WRAPPER(wait, (int *status), (status))
DEFINE_WRAPPER(get_pid, (void), ())
DEFINE_WRAPPER(yield, (void), ())

extern int64_t _spawn_process(char *name, int argc, char **argv, int redirect,
                              int fds[2]);

int64_t spawn_process_redirect(const char *name, int argc, char **argv,
                               int redirect, int red_fds[2]) {
  return _spawn_process(name, argc, argv, redirect, red_fds);
}

int64_t spawn_process(const char *name, int argc, char **argv) {
  return _spawn_process(name, argc, argv, 0, NULL);
}
