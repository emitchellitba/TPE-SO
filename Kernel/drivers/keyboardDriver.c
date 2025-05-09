#include <keyboardDriver.h>
#include <stdint.h>

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

static char buffer[5000];
unsigned int shift = 0;
unsigned int capsLock = 0;
unsigned int lastIndexBuffer = 0;
unsigned int readIndexBuffer = 0;
unsigned int arrow = 0;
extern unsigned int getScanCode();

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
    {0, 0},     {0, 0},      {0, 0},       {' ', ' '},
};

void press_key() {
  unsigned int scanCode = getScanCode();
  unsigned int col = 0;

  switch (scanCode) {
  case LEFT_SHIFT_PRESSED:  // left shift pressed
  case RIGHT_SHIFT_PRESSED: // right shift pressed
    shift = 1;
    break;
  case LEFT_SHIFT_RELEASED:  // left shift released
  case RIGHT_SHIFT_RELEASED: // right shift released
    shift = 0;
    break;
  case CAPSLOCK_PRESSED: // capsLock pressed
    if (capsLock == 1) {
      capsLock = 0;
    } else {
      capsLock = 1;
    }
    break;
  case ESC_PRESSED:
    esc = 1;
    break;
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

  if (scanCode <= 0x80) {
    if (arrow != 0) {
      buffer[lastIndexBuffer++] = arrow;
      arrow = 0;
    } else if (esc) {
      buffer[lastIndexBuffer++] = ESC;
    } else if (!(scanCode == LEFT_SHIFT_PRESSED ||
                 scanCode == RIGHT_SHIFT_PRESSED ||
                 scanCode == LEFT_SHIFT_RELEASED ||
                 scanCode == RIGHT_SHIFT_RELEASED ||
                 scanCode == CAPSLOCK_PRESSED)) {
      if (printableAscii[scanCode][0] >= 'a' &&
          printableAscii[scanCode][0] <= 'z') {
        if ((shift == 1 && capsLock == 0) || (capsLock == 1 && shift == 0)) {
          col = 1;
        }
      } else {
        if (shift == 1) {
          col = 1;
        }
      }
      buffer[lastIndexBuffer++] = printableAscii[scanCode][col];
    }
    lastIndexBuffer %= 5000;
  }
}

unsigned char getLastKey() {
  // si no hay caracteres por leer, devuelvo -1
  if (readIndexBuffer == lastIndexBuffer)
    return -1;
  if (buffer[readIndexBuffer] > 0x80) {
    readIndexBuffer++;
    return 0;
  }
  return buffer[readIndexBuffer++];
}

void load_buffer(char *buffer, size_t count) {
  char current;
  int myCount = 0;
  while ((current = getLastKey()) != -1 && myCount < count) {
    if (current != 0)
      buffer[myCount++] = current;
  }
}
