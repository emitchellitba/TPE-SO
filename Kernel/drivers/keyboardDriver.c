// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <keyboardDriver.h>

#include <lib.h>
#include <queue.h>
#include <ringbuf.h>
#include <scheduler.h>
#include <tty.h>
#include <videoDriver.h>

static void handle_ctrl_d();
static void handle_ctrl_c();

typedef enum { LEFT = 1, UP, DOWN, RIGHT } ARROWS;

#define LEFT_SHIFT_PRESSED 0x2A
#define LEFT_SHIFT_RELEASED 0xAA
#define RIGHT_SHIFT_PRESSED 0x36
#define RIGHT_SHIFT_RELEASED 0xB6
#define CAPSLOCK_PRESSED 0x3A
#define ESC_PRESSED 0x01
#define UP_ARROW 0x48
#define LEFT_ARROW 0x4B
#define RIGHT_ARROW 0x4D
#define DOWN_ARROW 0x50
#define ESC 5
#define KBUFF_SIZE 8192

extern unsigned int get_scan_code();

static int canonical_mode = 1;

struct ringbuf *kbuff = RINGBUF_NEW(KBUFF_SIZE);
unsigned int shift = 0;
unsigned int capsLock = 0;
unsigned int arrow = 0;
unsigned int ctrl = 0;

static int current_line_len = 0;

struct queue *waiting_queue = QUEUE_NEW();

static unsigned char printableAscii[58][2] = {
    {0, 0},     {27, 27},    {'1', '!'},   {'2', '@'}, {'3', '#'},   {'4', '$'},
    {'5', '%'}, {'6', '^'},  {'7', '&'},   {'8', '*'}, {'9', '('},   {'0', ')'},
    {'-', '_'}, {'=', '+'},  {'\b', '\b'}, {9, 9},     {'q', 'Q'},   {'w', 'W'},
    {'e', 'E'}, {'r', 'R'},  {'t', 'T'},   {'y', 'Y'}, {'u', 'U'},   {'i', 'I'},
    {'o', 'O'}, {'p', 'P'},  {'[', '{'},   {']', '}'}, {'\n', '\n'}, {0, 0},
    {'a', 'A'}, {'s', 'S'},  {'d', 'D'},   {'f', 'F'}, {'g', 'G'},   {'h', 'H'},
    {'j', 'J'}, {'k', 'K'},  {'l', 'L'},   {';', ':'}, {39, 34},     {'`', '~'},
    {0, 0},     {'\\', '|'}, {'z', 'Z'},   {'x', 'X'}, {'c', 'C'},   {'v', 'V'},
    {'b', 'B'}, {'n', 'N'},  {'m', 'M'},   {',', '<'}, {'.', '>'},   {'/', '?'},
    {0, 0},     {0, 0},      {0, 0},       {' ', ' '}};

/** No deberia ejecutarse si no hay una linea disponible */
static int get_line(char *buffer, size_t count) {
  if (!canonical_mode) {
    return ringbuf_read(kbuff, count, buffer);
  }

  int n = ringbuf_read_until(kbuff, buffer, count, '\n');
  if (n == count && buffer[count - 1] != '\n') {
    char rest_buffer[128];
    ringbuf_read_until(kbuff, rest_buffer, KBUFF_SIZE, '\n');
  }
  return n;
}

static int next_line_length() {
  if (canonical_mode)
    return ringbuf_find(kbuff, '\n');
  else
    return ringbuf_available(kbuff);
}

/**
 * Funcion que expone el driver para leer una linea de teclado.
 * Esta funcion solo debe ser llamada por el proceso que tenga el foreground.
 */
int read_line(char *buffer, size_t count) {
  while (next_line_length() == 0) {
    if (waiting_queue->count > 0) {
      return -1;
    }
    enqueue(waiting_queue, get_running());
    block_current(BLK_TERMINAL, waiting_queue);
  }

  return get_line(buffer, count);
}

void handle_key_press() {
  unsigned int scanCode = get_scan_code();

  switch (scanCode) {
  case 0x1D:
    ctrl = 1;
    return;
  case 0x9D:
    ctrl = 0;
    return;
  case LEFT_SHIFT_PRESSED:
  case RIGHT_SHIFT_PRESSED:
    shift = 1;
    return;
  case LEFT_SHIFT_RELEASED:
  case RIGHT_SHIFT_RELEASED:
    shift = 0;
    return;
  case CAPSLOCK_PRESSED:
    capsLock = !capsLock;
    return;
  case ESC_PRESSED: {
    esc = 1;
    return;
  }
  case LEFT_ARROW:
    arrow = LEFT;
    break;
  case UP_ARROW:
    arrow = UP;
    break;
  case DOWN_ARROW:
    arrow = DOWN;
    break;
  case RIGHT_ARROW:
    arrow = RIGHT;
    break;
  default:
    break;
  }

  if (scanCode > 0x80)
    return;

  if (scanCode >= (sizeof(printableAscii) / sizeof(printableAscii[0])))
    return;

  if (ctrl) {
    unsigned char c = printableAscii[scanCode][0];

    if (c == 'd') { /* CTRL + D */
      print_str("\n^D", 3, 0);

      handle_ctrl_d();
      return;
    }

    if (c == 'c') { /* CTRL + C */
      print_str("\n^C", 3, 0);

      handle_ctrl_c();
      return;
    }
  }

  if (arrow) {
    arrow = 0;
    return;
  }

  unsigned int col = shift ^ capsLock ? 1 : 0;
  unsigned char c = printableAscii[scanCode][col];

  if (c == '\b') {
    if (current_line_len > 0) {
      ringbuf_unwrite(kbuff);
      print_str((char *)&c, 1, 0);
      current_line_len--;
    }

    return;
  }

  if (c == '\t') {
    for (int i = 0; i < 4; i++) {
      ringbuf_write(kbuff, 1, " ");
      print_str(" ", 1, 0);
      current_line_len++;
    }
    return;
  }

  if ((c >= 32 && c <= 126)) {
    ringbuf_write(kbuff, 1, (char *)&c);
    print_str((char *)&c, 1, 0);
    current_line_len++;
  }

  if (c == '\n') {
    ringbuf_write(kbuff, 1, (char *)&c);
    print_str((char *)&c, 1, 0);

    if (canonical_mode) {
      if (waiting_queue->count > 0) {
        proc_ready(dequeue(waiting_queue));
      }
    }

    current_line_len = 0;
  }

  if (ringbuf_available(kbuff) && !canonical_mode && waiting_queue->count > 0)
    proc_ready(dequeue(waiting_queue));
}

static void handle_ctrl_d() {
  if (waiting_queue->count > 0) {
    if (current_line_len == 0) {
      char veof = VEOF;
      ringbuf_write(kbuff, 1, &veof);
    }

    char nl = '\n';
    ringbuf_write(kbuff, 1, &nl);
    proc_ready(dequeue(waiting_queue));
  }
  return;
}

static void handle_ctrl_c() {
  signal_kill_tty();
  return;
}

void set_canonical_mode(int mode) {
  if (mode == 0 || mode == 1) {
    canonical_mode = mode;
  }
}

int get_canonical_mode() { return canonical_mode; }
