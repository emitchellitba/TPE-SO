#include <keyboardDriver.h>

#include <lib.h>
#include <ringbuf.h>
#include <scheduler.h>
#include <videoDriver.h>

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

typedef struct ReadRequest {
  proc_t *proc;      // Proceso esperando
  char *user_buffer; // Dónde copiar la entrada
  int to_read;       // Cuántos caracteres quiere leer
} read_request_t;

struct ringbuf *kbuff = RINGBUF_NEW(KBUFF_SIZE);
unsigned int shift = 0;
unsigned int capsLock = 0;
unsigned int arrow = 0;

static struct queue *kbd_read_queue = QUEUE_NEW(); /* Kbd read wait queue */

int isReading = 0;

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
  return ringbuf_read_until(kbuff, buffer, count, '\n');
}

static int next_line_length() { return ringbuf_find(kbuff, '\n'); }

int read_line(char *buffer, size_t count) {
  if (next_line_length() == 0) {
    read_request_t *request = kmalloc(kernel_mem, sizeof(read_request_t));
    if (!request) {
      printk("Error: No se pudo asignar memoria para ReadRequest.\n");
      return -1;
    }

    request->proc = get_running();
    request->user_buffer = buffer;
    request->to_read = count;

    enqueue(kbd_read_queue, request);
    block_current(0, NULL);

    return READ_LINE_BLOCKED;
  } else {
    int read = get_line(buffer, count);
  }
}

void handle_key_press() {
  unsigned int scanCode = get_scan_code();

  switch (scanCode) {
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
    char aux = ESC;
    ringbuf_write(kbuff, 1, &aux);
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

  if (arrow) {
    ringbuf_write(kbuff, 1, (char *)&arrow);
    arrow = 0;
    return;
  }

  if (scanCode == LEFT_SHIFT_PRESSED || scanCode == RIGHT_SHIFT_PRESSED ||
      scanCode == LEFT_SHIFT_RELEASED || scanCode == RIGHT_SHIFT_RELEASED ||
      scanCode == CAPSLOCK_PRESSED)
    return;

  unsigned int col = shift ^ capsLock ? 1 : 0;
  unsigned char c = printableAscii[scanCode][col];
  ringbuf_write(kbuff, 1, (char *)&c);

  if ((c >= 32 && c <= 126) || c == '\n' || c == '\b' || c == 9)
    print_str((char *)&c, 1, 0);

  if (c == '\n') {
    while (!queue_is_empty(kbd_read_queue) && next_line_length() > 0) {
      read_request_t *req = (read_request_t *)dequeue(kbd_read_queue);

      int n = get_line(req->user_buffer, req->to_read);

      proc_ready(req->proc);

      return_from_syscall(req->proc, n);
    }
  }
}
