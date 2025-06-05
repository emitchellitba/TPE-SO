#include <utils.h>

uint32_t uint_to_base(uint64_t value, char *buffer, uint32_t base) {
  char *p = buffer, *p1, *p2;
  uint32_t digits = 0;
  do {
    uint32_t remainder = value % base;
    *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
    digits++;
  } while (value /= base);
  *p = 0;
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
