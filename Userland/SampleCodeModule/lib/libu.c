// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <libu.h>

DEFINE_WRAPPER(get_date_time, (const void *dt), (dt))
DEFINE_WRAPPER(screen_clear, (void), ())
DEFINE_WRAPPER(change_color, (uint8_t background), (background))
// DEFINE_WRAPPER(opCodeExc, (void), ())
DEFINE_WRAPPER(sleep_time, (uint64_t time), (time))
DEFINE_WRAPPER(usleep_time, (uint64_t time), (time))
DEFINE_WRAPPER(zoom, (int in), (in))
DEFINE_WRAPPER(get_regist, (uint64_t * regs), (regs))
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
DEFINE_WRAPPER(set_canonical_mode, (int enable), (enable))
DEFINE_WRAPPER(get_tty_mode, (void), ())
DEFINE_WRAPPER(m_malloc, (size_t size), (size))
DEFINE_WRAPPER(m_free, (void *ptr), (ptr))
DEFINE_WRAPPER(mem_dump, (void), ())
DEFINE_WRAPPER(my_sem_create, (uint64_t id, uint64_t initial_value),
               (id, initial_value))
DEFINE_WRAPPER(my_sem_open, (uint64_t id), (id))
DEFINE_WRAPPER(my_sem_close, (semaphore_t * sem), (sem))
DEFINE_WRAPPER(my_sem_post, (semaphore_t * sem), (sem))
DEFINE_WRAPPER(my_sem_wait, (semaphore_t * sem), (sem))
DEFINE_WRAPPER(my_sem_trywait, (semaphore_t * sem), (sem))

extern int64_t _spawn_process(const char *name, int argc, char *argv[],
                              int fds[], int background);

int64_t spawn_process(const char *name, int argc, char *argv[], int fds[]) {
  return _spawn_process(name, argc, argv, fds, 0);
}

int64_t spawn_process_bg(const char *name, int argc, char *argv[], int fds[]) {
  return _spawn_process(name, argc, argv, fds, 1);
}
