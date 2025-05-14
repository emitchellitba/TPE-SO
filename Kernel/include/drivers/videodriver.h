#ifndef _VIDEODRIVER_H_
#define _VIDEODRIVER_H_

#include <../interrupts/sysCallDispatcher.h>
#include <stdint.h>

#define SCREEN_WIDTH_PIXELS 1024
#define SCREEN_HEIGHT_PIXELS 768

typedef struct {
  int x;
  int y;
} Segment;

typedef uint32_t color;

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
void printStd(const char *c, size_t count);
void printErr(const char *c, size_t count);
void drawcharSize(unsigned char c, color fcolor, color bcolor);
void print(const char *c, size_t count, FDS fd);
void print_hex(uint64_t num);
void clear_screen();
uint8_t getHexDigit(uint64_t number, int position);
void change_font_color();
void change_bg_color();
void draw_sqr(int x, int y, uint64_t col, uint64_t size);
void zoom_in();
void zoom_out();
void free_draw(int x, int y, int drawing[][28], color *colors, int size);

#endif