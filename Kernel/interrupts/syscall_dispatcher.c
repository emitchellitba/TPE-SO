// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <syscall_dispatcher.h>

#include <process.h>

static void fill_out_buffer(uint64_t *buffer);

static int syscall_log_level = LOG_DEBUG;
LOGGER_DEFINE(syscall, syscall_log, syscall_log_level)

// TODO: chequear que retornamos cuando hay un error!
extern uint64_t register_array[];

typedef int64_t (*syscall_func_t)(va_list);

/*
SYSCALLS FALTANTES
● Crear y finalizar un proceso. deberá soportar el pasaje de parámetros.
● Obtener el ID del proceso que llama.
● Listar todos los procesos: nombre, ID, prioridad, stack y base pointer,
foreground y cualquier otra variable que consideren necesaria. ● Matar un
proceso arbitrario. ● Modificar la prioridad de un proceso arbitrario. ●
Bloquear y desbloquear un proceso arbitrario. ● Renunciar al CPU. ● Esperar a
que los hijos terminen.*/

static syscall_func_t syscall_table[] = {
    sys_read,            // 0
    sys_write,           // 1
    sys_clear_screen,    // 2
    sys_change_color,    // 3
    sys_get_time,        // 4
    sys_sleep,           // 5
    sys_usleep,          // 6
    sys_zoom,            // 7
    sys_fill_out_buffer, // 8
    sys_beep,            // 9
    sys_read_kmsg,       // 10
    sys_pipe_create,     // 11
    sys_pipe_open,       // 12
    sys_pipe_close,      // 13
    sys_get_procs,       // 14
    sys_load_program,    // 15
    sys_rm_program,      // 16
    sys_get_programs,    // 17
    sys_spawn_process,   // 18
    sys_kill_proc,       // 19
    sys_change_priority, // 20
    sys_exit,            // 21
    sys_block,           // 22
    sys_unblock,         // 23
    sys_copy_fd,         // 24
    sys_close_fd,        // 25
    sys_wait_pid,        // 26
    sys_wait,            // 27
    sys_get_pid,         // 28
    sys_yield,           // 29
    sys_malloc,          // 30
    sys_free,            // 31
    sys_mem_dump,        // 32
    sys_set_canonical,   // 33
    sys_get_tty_mode,    // 34
    sys_create_sem,      // 35
    sys_open_sem,        // 36
    sys_close_sem,       // 37
    sys_sem_post,        // 38
    sys_sem_wait,        // 39
    sys_sem_trywait,     // 40
};

#define NUM_SYSCALLS (sizeof(syscall_table) / sizeof(syscall_table[0]))

int64_t syscall_dispatcher(uint64_t rax, ...) {
  if (rax >= NUM_SYSCALLS) {
    return INVALID_SYS_ID;
  }
  va_list args;
  va_start(args, rax);
  int64_t ret_val = syscall_table[rax](args);
  va_end(args);
  return ret_val;
}

int64_t sys_read(va_list args) {
  int fd = va_arg(args, int);
  char *buffer = va_arg(args, char *);
  uint64_t count = va_arg(args, uint64_t);

  syscall_log(LOG_INFO, "read(fd=%d, buffer=%p, count=%lu)\n", fd, buffer,
              count);

  if (fd < 0 || fd >= FD_MAX) {
    syscall_log(LOG_ERR, "Invalid file descriptor: %d\n", fd);
    return -EINVAL;
  }

  proc_t *current_process = get_running();

  if (!current_process) {
    syscall_log(LOG_ERR, "No current process in scheduler\n");
    return -EFAULT;
  }

  // TODO: Hacer una funcion get_fd_entry(current_process, fd);
  // que devuelva un puntero a fd_entry_t
  fd_entry_t *fd_entry = &current_process->fds[fd];

  if (!fd_entry->ops || !fd_entry->ops->read) {
    return -EBADF;
  }

  return fd_entry->ops->read(fd_entry->resource, buffer, count);
}

int64_t sys_write(va_list args) {
  int fd = va_arg(args, int);
  const char *buffer = va_arg(args, const char *);
  uint64_t count = va_arg(args, uint64_t);

  syscall_log(LOG_INFO, "write(fd=%d, buffer=%p, count=%lu)\n", fd, buffer,
              count);

  if (fd < 0 || fd >= FD_MAX) {
    syscall_log(LOG_ERR, "Invalid file descriptor: %d\n", fd);
    return -EINVAL;
  }

  proc_t *current_process = get_running();

  if (!current_process) {
    syscall_log(LOG_ERR, "No current process in scheduler\n");
    return -EFAULT;
  }

  fd_entry_t *fd_entry = &current_process->fds[fd];

  if (!fd_entry->ops || !fd_entry->ops->write) {
    return -EBADF;
  }

  return fd_entry->ops->write(fd_entry->resource, buffer, count);
}

int64_t sys_clear_screen(va_list args) {
  (void)args;
  syscall_log(LOG_INFO, "clear_screen()\n");
  clear_screen();
  return 0;
}

int64_t sys_change_color(va_list args) {
  uint8_t background = (uint8_t)va_arg(args, int); // promoted to int
  syscall_log(LOG_INFO, "change_color(background=%d)\n", background);
  if (background)
    change_bg_color();
  else
    change_font_color();
  return 0;
}

int64_t sys_get_time(va_list args) {
  date_time *dt = va_arg(args, date_time *);
  syscall_log(LOG_INFO, "get_time(dt=%p)\n", dt);
  get_time(dt);
  return 0;
}

int64_t sys_sleep(va_list args) {
  uint64_t ticks = va_arg(args, uint64_t);
  syscall_log(LOG_INFO, "sleep(ticks=%u)\n", ticks);
  sleep(ticks);
  return 0;
}

int64_t sys_usleep(va_list args) {
  uint64_t microseconds = va_arg(args, uint64_t);
  syscall_log(LOG_INFO, "usleep(microseconds=%u)\n", microseconds);
  usleep(microseconds);
  return 0;
}

int64_t sys_zoom(va_list args) {
  int64_t in = va_arg(args, int64_t);
  syscall_log(LOG_INFO, "zoom(in=%ld)\n", in);
  if (in)
    zoom_in();
  else
    zoom_out();
  return 0;
}

int64_t sys_fill_out_buffer(va_list args) {
  uint64_t *bufferRegs = va_arg(args, uint64_t *);
  syscall_log(LOG_INFO, "fill_out_buffer(bufferRegs=%p)\n", bufferRegs);
  fill_out_buffer(bufferRegs);
  return 0;
}

int64_t sys_beep(va_list args) {
  int64_t time = va_arg(args, int64_t);
  int64_t nFrequence = va_arg(args, int64_t);
  syscall_log(LOG_INFO, "beep(time=%ld, nFrequence=%ld)\n", time, nFrequence);
  beep(time, nFrequence);
  return 0;
}

int64_t sys_read_kmsg(va_list args) {
  char *user_buf = va_arg(args, char *);
  uint64_t maxlen = va_arg(args, uint64_t);

  syscall_log(LOG_INFO, "get_kmsg(user_buf=%p, maxlen=%d)\n", user_buf, maxlen);

  extern struct ringbuf *kmsg;

  size_t available = ringbuf_available(kmsg);

  size_t to_read = available < maxlen ? available : maxlen;
  ringbuf_read_noconsume(kmsg, 0, to_read, user_buf);

  if (to_read < maxlen) {
    user_buf[to_read] = '\0';
  } else {
    user_buf[maxlen - 1] = '\0';
  }

  return (int64_t)to_read;
}

int64_t sys_pipe_create(va_list args) {
  char *id = va_arg(args, char *);
  syscall_log(LOG_INFO, "pipe_create(id=%s)\n", id);

  pipe_t *pipe = create_pipe(id);
  if (!pipe)
    return -ENOMEM;

  return 0;
}

int64_t sys_pipe_open(va_list args) {
  char *id = va_arg(args, char *);
  int64_t mode = va_arg(args, int64_t);
  syscall_log(LOG_INFO, "pipe_open(id=%s, mode=%ld)\n", id, mode);

  if ((mode != PIPE_READ && mode != PIPE_WRITE) || !id)
    return -EINVAL;

  proc_t *current_process = get_running();
  if (!current_process)
    return -EFAULT;

  int free_fd = find_free_fd(current_process);
  if (free_fd < 0)
    return -ENOENT;

  int res = open_pipe(&current_process->fds[free_fd], id, mode);
  if (res < 0)
    return res;

  return free_fd;
}

int64_t sys_pipe_close(va_list args) {
  char *pipe_id = va_arg(args, char *);
  syscall_log(LOG_INFO, "pipe_close(pipe_id=%ld)\n", pipe_id);

  pipe_t *pipe = find_pipe_by_id(pipe_id);
  if (!pipe)
    return -ENOENT;

  pipe_free(pipe);

  return 0;
}

int64_t sys_get_procs(va_list args) {
  proc_info_t *buffer = va_arg(args, proc_info_t *);
  uint64_t count = va_arg(args, uint64_t);

  int out_count = 0;

  proc_list(buffer, count, &out_count);

  syscall_log(LOG_INFO, "ps()\n");
  return out_count;
}

int64_t sys_load_program(va_list args) {
  char *name = va_arg(args, char *);
  uint64_t entry = va_arg(args, uint64_t);
  syscall_log(LOG_INFO, "load_program(name=%s, entry=%ld)\n", name, entry);

  int err = 0;

  if ((err = fs_load(name, (fs_entry_point_t)entry))) {
    syscall_log(LOG_CRIT, "failed to load program %s with entry %ld\n", name,
                entry);
    return -1;
  }

  syscall_log(LOG_INFO, "program %s loaded successfully\n", name);
  return 0;
}

int64_t sys_rm_program(va_list args) {
  char *name = va_arg(args, char *);
  syscall_log(LOG_INFO, "rm_program(name=%s)\n", name);

  int err = 0;
  if ((err = fs_rm(name))) {
    syscall_log(LOG_CRIT, "failed to remove program %s\n", name);
    return -ENOENT;
  }

  syscall_log(LOG_INFO, "program %s removed successfully\n", name);
  return 0;
}

int64_t sys_get_programs(va_list args) {
  /* recibe un buffer de strings y una cantidad maxima */
  char(*buffer)[MAX_FILE_NAME_LEN] = va_arg(args, char(*)[MAX_FILE_NAME_LEN]);
  int max_count = va_arg(args, int);
  syscall_log(LOG_INFO, "ls_programs()\n");

  int out_count = 0;

  if (fs_list_programs(buffer, max_count, &out_count) < 0) {
    syscall_log(LOG_CRIT, "failed to list programs\n");
    return -EFAULT;
  }

  return out_count;
}

int64_t sys_spawn_process(va_list args) {
  char *name = va_arg(args, char *);
  int argc = va_arg(args, int);
  char **argv = va_arg(args, char **);
  int *fds = va_arg(args, int *);
  int background = va_arg(args, int);
  syscall_log(LOG_INFO, "execv(name=%s, argc=%d, argv=%p)\n", name, argc, argv);

  int err = 0;
  proc_t *new_proc;

  fs_entry_point_t entry = fs_get_entry(name);

  if (!entry) {
    syscall_log(LOG_CRIT, "program %s not found\n", name);
    return -ENOENT;
  }

  if ((err = proc_new(&new_proc))) {
    syscall_log(LOG_CRIT, "failed to allocate process structure for init");
    return -ENOMEM;
  }

  proc_init(new_proc, name, (proc_main_function)entry, fds, background);

  execv(new_proc, argc, argv);

  proc_ready(new_proc);

  return new_proc->pid;
}

int64_t sys_kill_proc(va_list args) {
  int64_t pid = va_arg(args, int64_t);

  syscall_log(LOG_INFO, "kill_proc(pid=%ld)\n", pid);

  if (pid == 0 || pid == 1)
    return -EPERM;

  proc_t *proc = get_proc_by_pid(pid);
  if (proc == NULL)
    return -ESRCH;

  proc_kill(proc, 128 + SIGKILL);

  return 0;
}

// TODO: Mover esta funcion
static void fill_out_buffer(uint64_t *buffer) {
  for (int64_t i = 0; i < 17; i++) {
    buffer[i] = register_array[i];
  }
}

int64_t sys_change_priority(va_list args) {
  int64_t pid = va_arg(args, int64_t);
  int64_t new_priority = va_arg(args, int64_t);
  syscall_log(LOG_INFO, "change_priority(pid=%ld, new_priority=%d)\n", pid,
              new_priority);

  proc_t *proc = get_proc_by_pid(pid);

  return change_priority(proc, (priority_t)new_priority);
}

int64_t sys_exit(va_list args) {
  int code = va_arg(args, int);
  syscall_log(LOG_DEBUG, "exit(code=%d)\n", code);

  proc_t *current_process = get_running();

  proc_kill(current_process, code);

  call_timer_tick();

  // No se deberia alcanzar este punto
  while (1)
    ;
}

int64_t sys_block(va_list args) {
  int64_t pid = va_arg(args, pid_t);
  syscall_log(LOG_INFO, "sys_block(pid=%ld)\n", pid);
  return block_process_by_pid((pid_t)pid);
}

int64_t sys_unblock(va_list args) {
  int64_t pid = va_arg(args, pid_t);
  syscall_log(LOG_INFO, "sys_unblock(pid=%ld)\n", pid);
  return unblock_process_by_pid((pid_t)pid);
}

int64_t sys_copy_fd(va_list args) { return 0; }

int64_t sys_close_fd(va_list args) {
  int fd = va_arg(args, int);
  syscall_log(LOG_INFO, "sys_close_fd(fd=%d)\n", fd);

  proc_t *current_process = get_running();
  if (!current_process)
    return -EFAULT;

  if (fd < 0 || fd >= FD_MAX)
    return -EINVAL;

  fd_entry_t *entry = &current_process->fds[fd];

  if (entry->ops && entry->ops->close) {
    entry->ops->close(entry->resource);
  }

  entry->ops = NULL;
  entry->resource = NULL;
  entry->type = FD_NONE;

  return 0;
}

int64_t sys_wait_pid(va_list args) {
  int64_t pid = va_arg(args, pid_t);
  int *exit_status = va_arg(args, int *);

  syscall_log(LOG_INFO, "sys_wait_pid(pid=%ld)\n", pid);

  return wait_pid(pid, exit_status);
}

int64_t sys_wait(va_list args) {
  int *exit_status = va_arg(args, int *);

  return wait_pid(WAIT_PID, exit_status);
}

int64_t sys_get_pid(va_list args) {
  syscall_log(LOG_INFO, "sys_get_pid()\n");
  proc_t *current_process = get_running();
  return current_process->pid;
}

int64_t sys_yield(va_list args) {
  (void)args; // No se usan argumentos
  syscall_log(LOG_INFO, "sys_yield()\n");
  yield();
  return 0;
}

int64_t sys_malloc(va_list args) {
  size_t size = va_arg(args, size_t);
  syscall_log(LOG_INFO, "sys_malloc(size=%zu)\n", size);
  return (int64_t)kmalloc(kernel_mem, size);
}

int64_t sys_free(va_list args) {
  void *ptr = va_arg(args, void *);
  syscall_log(LOG_INFO, "sys_free(ptr=%p)\n", ptr);
  kmm_free(kernel_mem, ptr);
  return 0;
}

int64_t sys_mem_dump(va_list args) {
  // Esta syscall es para depuración, no debería estar en producción
  (void)args; // No se usan argumentos
  syscall_log(LOG_INFO, "sys_mem_dump()\n");
  kmm_dump_state(kernel_mem);
  return 0;
}

int64_t sys_set_canonical(va_list args) {
  int enable = va_arg(args, int);
  syscall_log(LOG_INFO, "sys_set_canonical(enable=%d)\n", enable);

  if (enable) {
    set_canonical_mode(1);
  } else {
    set_canonical_mode(0);
  }

  return 0;
}

int64_t sys_get_tty_mode(va_list args) {
  (void)args;
  syscall_log(LOG_INFO, "sys_get_tty_mode()\n");

  return get_canonical_mode();
}

int64_t sys_create_sem(va_list args) {
  uint64_t id = va_arg(args, uint64_t);
  uint64_t initial_value = va_arg(args, uint64_t);
  syscall_log(LOG_INFO, "sys_create_sem(id=%d, initial_value=%d)\n", id,
              initial_value);

  return my_sem_create(id, initial_value);
}

int64_t sys_open_sem(va_list args) {
  uint64_t id = va_arg(args, uint64_t);
  syscall_log(LOG_INFO, "sys_open_sem(id=%d)\n", id);

  return my_sem_open(id);
}

int64_t sys_close_sem(va_list args) {
  uint64_t sem_id = va_arg(args, uint64_t);
  syscall_log(LOG_INFO, "sys_close_sem(sem_id=%d)\n", sem_id);

  return my_sem_destroy(sem_id);
}

int64_t sys_sem_post(va_list args) {
  uint64_t sem_id = va_arg(args, uint64_t);
  syscall_log(LOG_INFO, "sys_sem_post(sem_id=%d)\n", sem_id);

  return my_sem_post(sem_id);
}

int64_t sys_sem_wait(va_list args) {
  uint64_t sem_id = va_arg(args, uint64_t);
  syscall_log(LOG_INFO, "sys_sem_wait(sem_id=%d)\n", sem_id);

  return my_sem_wait(sem_id);
}

int64_t sys_sem_trywait(va_list args) {
  uint64_t sem_id = va_arg(args, uint64_t);
  return my_sem_trywait(sem_id);
}
