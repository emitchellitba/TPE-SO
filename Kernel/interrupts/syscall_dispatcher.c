#include <syscall_dispatcher.h>

// TODO: chequear que retornamos cuando hay un error!
extern uint64_t register_array[];

typedef int64_t (*syscall_func_t)(va_list);

static syscall_func_t syscall_table[NUM_SYSCALLS] = {
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
    sys_beep             // 10
};

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
    load_buffer(buffer, count);
    break;
  default:
    break;
  }
  return 1;
}

int64_t sys_write(va_list args) {
  FDS fd = va_arg(args, int);
  const uint8_t *buffer = va_arg(args, const uint8_t *);
  uint64_t count = va_arg(args, uint64_t);
  switch (fd) {
  case STDOUT:
    printStd(buffer, count);
    break;
  case STDERR:
    printErr(buffer, count);
    break;
  default:
    break;
  }
  return 1;
}

int64_t sys_clear_screen(va_list args) {
  (void)args;
  clear_screen();
  return 0;
}

int64_t sys_change_color(va_list args) {
  uint8_t background = (uint8_t)va_arg(args, int); // promoted to int
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
  draw_sqr(x, y, col, size);
  return 0;
}

int64_t sys_free_draw(va_list args) {
  int64_t x = va_arg(args, int64_t);
  int64_t y = va_arg(args, int64_t);
  int64_t **drawing = va_arg(args, int64_t **);
  color *colors = va_arg(args, color *);
  int64_t size = va_arg(args, int64_t);
  free_draw(x, y, drawing, colors, size);
  return 0;
}

int64_t sys_get_time(va_list args) {
  date_time *dt = va_arg(args, date_time *);
  get_time(dt);
  return 0;
}

int64_t sys_sleep(va_list args) {
  _sti();
  uint32_t ticks = va_arg(args, uint32_t);
  sleep(ticks);
  _cli();
  return 0;
}

int64_t sys_zoom(va_list args) {
  int64_t in = va_arg(args, int64_t);
  if (in)
    zoom_in();
  else
    zoom_out();
  return 0;
}

int64_t sys_fill_out_buffer(va_list args) {
  uint64_t *bufferRegs = va_arg(args, uint64_t *);
  fill_out_buffer(bufferRegs);
  return 0;
}

int64_t sys_beep(va_list args) {
  int64_t time = va_arg(args, int64_t);
  int64_t nFrequence = va_arg(args, int64_t);
  beep(time, nFrequence);
  return 0;
}

static void fill_out_buffer(uint64_t *buffer) {
  for (int64_t i = 0; i < 17; i++) {
    buffer[i] = register_array[i];
  }
}
