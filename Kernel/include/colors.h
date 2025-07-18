#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>

enum COLORS {
  WHITE = 0x00FFFFFF,
  BLACK = 0x00000000,
  RED = 0x00FF0000,
  GREY = 0x001A1F33,
  GREEN = 0x0000FF00,
  BLUE = 0x000000FF,
  YELLOW = 0x0000FFFF,
  LIGHT_BLACK = 0x000D1B2A
};

typedef uint32_t color;

#endif // COLORS_H
