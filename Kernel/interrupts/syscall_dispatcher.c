#include <proc/process.h>
#include <syscall_dispatcher.h>

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
    sys_draw_sqr,        // 4
    sys_free_draw,       // 5
    sys_get_time,        // 6
    sys_sleep,           // 7
    sys_zoom,            // 8
    sys_fill_out_buffer, // 9
    sys_beep,            // 10
    sys_read_kmsg,       // 11
    sys_pipe_open,       // 12
    sys_pipe_close,      // 13
    sys_ps,              // 14
    sys_new_proc,        // 15
    sys_kill_proc,       // 16
    sys_change_priority, // 17
                         // sys_wait,           // 18
                         // sys_set_priority,   // 19
                         // sys_block,         // 20
                         // sys_unblock,       // 21
                         // sys_renounce,      // 22
                         // sys_getpid,        // 23
                         // sys_getppid,       // 24
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
  FDS fd = va_arg(args, int); // enums are promoted to int
  uint8_t *buffer = va_arg(args, uint8_t *);
  uint64_t count = va_arg(args, uint64_t);
  switch (fd) {
  case STDIN:
    load_buffer((char *)buffer, count);
    break;
  default:
    break;
  }
  return 1;
}

int64_t sys_write(va_list args) {
  /* 1 y 2 son fds para STDOUT Y STDERR */

  FDS fd = va_arg(args, int);
  const uint8_t *buffer = va_arg(args, const uint8_t *);
  uint64_t count = va_arg(args, uint64_t);
  switch (fd) {
  case STDOUT:
    printStd((const char *)buffer, count);
    break;
  case STDERR:
    // TODO: Borrar casteo
    printErr((const char *)buffer, count);
    break;
  default:
    break;
  }
  return 1;
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

int64_t sys_draw_sqr(va_list args) {
  int64_t x = va_arg(args, int64_t);
  int64_t y = va_arg(args, int64_t);
  uint64_t col = va_arg(args, uint64_t);
  uint64_t size = va_arg(args, uint64_t);
  syscall_log(LOG_INFO, "draw_sqr(x=%ld, y=%ld, col=%lu, size=%lu)\n", x, y,
              col, size);
  draw_sqr(x, y, col, size);
  return 0;
}

int64_t sys_free_draw(va_list args) {
  int64_t x = va_arg(args, int64_t);
  int64_t y = va_arg(args, int64_t);
  int64_t **drawing = va_arg(args, int64_t **);
  color *colors = va_arg(args, color *);
  int64_t size = va_arg(args, int64_t);
  syscall_log(LOG_INFO,
              "free_draw(x=%ld, y=%ld, drawing=%p, colors=%p, size=%ld)\n", x,
              y, drawing, colors, size);
  // TODO: Borrar todos estos casteos y hacer tipos consistentes
  free_draw((int)x, (int)y, (int(*)[28])drawing, colors, (int)size);
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

int64_t sys_pipe_open(va_list args) {
  int64_t pipe_id = va_arg(args, int64_t);
  syscall_log(LOG_INFO, "pipe_open(pipe_id=%ld)\n", pipe_id);
  // return pipe_open(pipe_id);
}

int64_t sys_pipe_close(va_list args) {
  int64_t pipe_id = va_arg(args, int64_t);
  syscall_log(LOG_INFO, "pipe_close(pipe_id=%ld)\n", pipe_id);
  // return pipe_close(pipe_id);
}

int64_t sys_new_proc(va_list args) {
  char *name = va_arg(args, char *);
  int (*entry)(void) = va_arg(args, int (*)(void));
  int argc = va_arg(args, int);
  char **argv = va_arg(args, char **);
  syscall_log(LOG_INFO, "new_proc(name=%s, entry=%ld)\n", name, entry
              /* argv */);

  int err = 0;
  proc_t *new_proc;

  if ((err = proc_new(&new_proc))) {
    syscall_log(LOG_CRIT, "failed to allocate process structure for init");
    return -1;
  }

  proc_init(new_proc, name, NULL, entry);

  execv(new_proc, argc, argv);

  proc_ready(new_proc);

  return 0;
}

int64_t sys_kill_proc(va_list args) {
  int64_t pid = va_arg(args, int64_t);
  syscall_log(LOG_INFO, "kill_proc(pid=%ld)\n", pid);
  // return kill_proc(pid);
}

int64_t sys_ps(va_list args) {
  syscall_log(LOG_INFO, "ps()\n");
  // return ps();
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
