#include <naiveConsole.h>

static uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base);

static char buffer[64] = {'0'};
static uint8_t *const video = (uint8_t *)0xB8000;
static uint8_t *currentVideo = (uint8_t *)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25;

void ncPrint(const char *string) {
  int i;

  for (i = 0; string[i] != 0; i++)
    ncPrintChar(string[i]);
}

void ncPrintWC(const char *string, uint8_t background, uint8_t foreground) {
  int i;

  for (i = 0; string[i] != 0; i++)
    ncPrintCharWC(string[i], background, foreground);
}

void ncPrintChar(char character) {
  if (character == 9) {
    *currentVideo = ' ';
    currentVideo += 2;
    *currentVideo = ' ';
    currentVideo += 2;
    *currentVideo = ' ';
    currentVideo += 2;
    *currentVideo = ' ';
    currentVideo += 2;
  } else if (character == '\n') {
    do {
      *currentVideo = ' ';
      currentVideo += 2;
    } while ((uint64_t)(currentVideo - video) % (width * 2) != 0);
  } else if (character == '\r') {
    currentVideo -= (uint64_t)(currentVideo - video) % (width * 2);
  } else if (character == '\b') {
    currentVideo -= 2;
    *currentVideo = ' ';
    currentVideo += 2;
  } else if (character == '\0') {
    return;
  } else {
    *currentVideo = character;
    currentVideo += 2;
  }
}

void ncPrintCharWC(char character, uint8_t background, uint8_t foreground) {
  *currentVideo = character;
  currentVideo += 1;
  uint8_t num = (background << 4) | (foreground & 0x0F);
  *currentVideo = num;
  currentVideo += 1;
}

void ncNewline() {
  do {
    ncPrintChar(' ');
  } while ((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncPrintCharStyle(char character, char style) {
  *currentVideo++ = character;
  *currentVideo++ = style;
}

void ncPrintStyle(const char *string, char style) {
  int i;
  for (i = 0; string[i] != 0; i++)
    ncPrintCharStyle(string[i], style);
}

void ncPrintDec(uint64_t value) { ncPrintBase(value, 10); }

void ncPrintHex(uint64_t value) { ncPrintBase(value, 16); }

void ncPrintBin(uint64_t value) { ncPrintBase(value, 2); }

void ncPrintBase(uint64_t value, uint32_t base) {
  uintToBase(value, buffer, base);
  ncPrint(buffer);
}

void ncClear() {
  int i;

  for (i = 0; i < height * width; i++)
    video[i * 2] = ' ';
  currentVideo = video;
}

void ncDelete() {
  currentVideo -= 2;
  ncPrintChar(' ');
  currentVideo -= 2;
}

static uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base) {
  char *p = buffer;
  char *p1, *p2;
  uint32_t digits = 0;

  // Calculate characters for each digit
  do {
    uint32_t remainder = value % base;
    *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
    digits++;
  } while (value /= base);

  // Terminate string in buffer.
  *p = 0;

  // Reverse string in buffer.
  p1 = buffer;
  p2 = p - 1;
  while (p1 < p2) {
    char tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
    p1++;
    p2--;
  }

  return digits;
}
