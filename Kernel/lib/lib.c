#include <lib.h>
#include <stdint.h>

extern uint16_t getSec();
extern uint16_t getMin();
extern uint16_t getHour();
extern uint16_t getDay();
extern uint16_t getMonth();
extern uint16_t getYear();

void *memset(void *destination, int32_t c, uint64_t length) {
  uint8_t chr = (uint8_t)c;
  char *dst = (char *)destination;

  while (length--)
    dst[length] = chr;

  return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length) {
  /*
   * memcpy does not support overlapping buffers, so always do it
   * forwards. (Don't change this without adjusting memmove.)
   *
   * For speedy copying, optimize the common case where both pointers
   * and the length are word-aligned, and copy word-at-a-time instead
   * of byte-at-a-time. Otherwise, copy by bytes.
   *
   * The alignment logic below should be portable. We rely on
   * the compiler to be reasonably intelligent about optimizing
   * the divides and modulos out. Fortunately, it is.
   */
  uint64_t i;

  if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
      (uint64_t)source % sizeof(uint32_t) == 0 &&
      length % sizeof(uint32_t) == 0) {
    uint32_t *d = (uint32_t *)destination;
    const uint32_t *s = (const uint32_t *)source;

    for (i = 0; i < length / sizeof(uint32_t); i++)
      d[i] = s[i];
  } else {
    uint8_t *d = (uint8_t *)destination;
    const uint8_t *s = (const uint8_t *)source;

    for (i = 0; i < length; i++)
      d[i] = s[i];
  }

  return destination;
}

int str_len(const char *str) {
  int len = 0;
  while (*str != '\0') {
    len++;
    str++;
  }
  return len;
}

char *str_cpy(char *dest, const char *src) {
  char *start = dest;
  while (*src != '\0') {
    *dest++ = *src++;
  }
  *dest = '\0';
  return start;
}

int abs(int x) { return x < 0 ? -x : x; }

uint16_t getSecs() { return getSec(); }
uint16_t getMins() { return getMin(); }
uint16_t getHours() { return getHour(); }
uint16_t getDays() { return getDay(); }
uint16_t getMonths() { return getMonth(); }
uint16_t getYears() { return getYear(); }
