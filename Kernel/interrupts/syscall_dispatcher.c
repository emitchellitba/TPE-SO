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
    sys_zoom,            // 6
    sys_fill_out_buffer, // 7
    sys_beep,            // 8
    sys_read_kmsg,       // 9
    sys_pipe_create,     // 10
    sys_pipe_open,       // 11
    sys_pipe_close,      // 12
    sys_get_procs,       // 13
    sys_load_program,    // 14
    sys_rm_program,      // 15
    sys_get_programs,    // 16
    sys_spawn_process,   // 17
    sys_kill_proc,       // 18
    sys_change_priority, // 19
    sys_exit,            // 20
    sys_block,           // 21
    sys_unblock,         // 22
    sys_copy_fd,         // 23
    sys_close_fd,        // 24
    sys_wait_pid,        // 25
    sys_wait,            // 26
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

  if (!fd_entry || !fd_entry->ops || !fd_entry->ops->read) {
    return -EBADF;
  }

  int n = fd_entry->ops->read(fd_entry->resource, buffer, count);

  if (n == READ_LINE_BLOCKED) {
    return current_process->syscall_retval;
  }

  return n;
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

  if (!fd_entry || !fd_entry->ops || !fd_entry->ops->write) {
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
  _sti();
  uint32_t ticks = va_arg(args, uint32_t);
  syscall_log(LOG_INFO, "sleep(ticks=%u)\n", ticks);
  sleep(ticks);
  _cli();
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
  int redirect = va_arg(args, int);
  int *fds = va_arg(args, int *);
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

  proc_init(new_proc, name, entry, redirect, fds);

  execv(new_proc, argc, argv);

  proc_ready(new_proc);

  return new_proc->pid;
}

int64_t sys_kill_proc(va_list args) {
  int64_t pid = va_arg(args, int64_t);
  syscall_log(LOG_INFO, "kill_proc(pid=%ld)\n", pid);
  // return kill_proc(pid);
}

static void fill_out_buffer(uint64_t *buffer) {
  for (int64_t i = 0; i < 17; i++) {
    buffer[i] = register_array[i];
  }
}

int64_t sys_change_priority(va_list args) {
  int64_t pid = va_arg(args, int64_t);
  priority_t new_priority = va_arg(args, priority_t);
  syscall_log(LOG_INFO, "change_priority(pid=%ld, new_priority=%d)\n", pid,
              new_priority);
  return change_priority(pid, new_priority);
}

int64_t sys_exit(va_list args) {
  int code = va_arg(args, int);
  syscall_log(LOG_DEBUG, "exit(code=%d)\n", code);

  proc_t *current_process = get_running();

  if (!current_process) {
    syscall_log(LOG_CRIT, "exit called with no current process\n");
    return -EFAULT;
  }

  current_process->exit = code;

  proc_kill(current_process);

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

int64_t sys_copy_fd(va_list args) {
  //
}

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
