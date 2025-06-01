/* _loader.c */
#include <init.h>
#include <libu.h>
#include <stdint.h>

extern char bss;
extern char endOfBinary;

void *memset(void *destiny, int32_t c, uint64_t length);

int _start() {
  // Clean BSS
  memset(&bss, 0, &endOfBinary - &bss);

  return init_main(0, NULL);
}

void *memset(void *destiation, int32_t c, uint64_t length) {
  uint8_t chr = (uint8_t)c;
  char *dst = (char *)destiation;

  while (length--)
    dst[length] = chr;

  return destiation;
}
