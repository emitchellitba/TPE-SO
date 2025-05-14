#include <interrupts.h>
#include <keyboardDriver.h>
#include <lib.h>
#include <stdarg.h>
#include <stdint.h>
#include <sysCallDispatcher.h>
#include <time_rtc.h>
#include <videodriver.h>

// chequear que retornamos cuando hay un error!
extern uint64_t register_array[];

extern void beep(int ticks, int nFrequence);
void fill_out_buffer(uint64_t *buffer);

size_t sys_clear_screen() {
  clear_screen();
  return 0;
}

size_t sys_read(FDS fd, char *buffer, size_t count) {
  switch (fd) {
  case STDIN:
    load_buffer(buffer, count);
    break;
  default:
    break;
  }
  return 1;
}

size_t sys_write(FDS fd, const char *buffer, size_t count) {
  switch (fd) {
  case STDOUT: {
    printStd(buffer, count);
    break;
  }
  case STDERR: {
    printErr(buffer, count);
    break;
  }
  default:
    break;
  }
  return 1;
}

size_t sys_change_color(uint8_t background) {
  if (background)
    change_bg_color();
  else
    change_font_color();
  return 0;
}

void syscallDispatcher(uint64_t rax, ...) {
  va_list args;
  va_start(args, rax);
  switch (rax) {
  case 0: {
    FDS fd = va_arg(args, FDS);
    char *buffer = va_arg(args, char *);
    size_t count = va_arg(args, size_t);
    sys_read(fd, buffer, count);
    break;
  }
  case 1: {
    FDS fd = va_arg(args, FDS);
    const char *buffer = va_arg(args, const char *);
    size_t count = va_arg(args, size_t);
    sys_write(fd, buffer, count);
    break;
  }
  case 2: {
    sys_clear_screen();
    break;
  }
  case 3: {
    int background = va_arg(args, int);
    sys_change_color(background);
    break;
  }
  case 4: {
    int x = va_arg(args, int);
    int y = va_arg(args, int);
    uint64_t col = va_arg(args, uint64_t);
    uint64_t size = va_arg(args, uint64_t);
    draw_sqr(x, y, col, size);
    break;
  }
  case 5: {
    int x = va_arg(args, int);
    int y = va_arg(args, int);
    int **drawing = va_arg(args, int **);
    color *colors = va_arg(args, color *);
    int size = va_arg(args, int);
    free_draw(x, y, drawing, colors, size);
    break;
  }
  case 6: {
    date_time *dt = va_arg(args, date_time *);
    get_time(dt);
    break;
  }
  case 7: {
    _sti();
    uint32_t ticks = va_arg(args, uint32_t);
    sleep(ticks);
    _cli();
    break;
  }
  case 8: {
    int in = va_arg(args, int);
    if (in)
      zoom_in();
    else
      zoom_out();
    break;
  }
  case 9: {
    uint64_t *bufferRegs = va_arg(args, uint64_t *);
    fill_out_buffer(bufferRegs);
  } break;
  case 10: {
    int time = va_arg(args, int);
    int nFrequence = va_arg(args, int);
    beep(time, nFrequence);
  } break;
  }
  va_end(args);
  return;
}

void fill_out_buffer(uint64_t *buffer) {
  for (int i = 0; i < 17; i++) {
    buffer[i] = register_array[i];
  }
}
