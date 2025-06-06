#ifndef _VIDEODRIVER_H_
#define _VIDEODRIVER_H_

#include <colors.h>
#include <lib.h>
#include <stddef.h>
#include <stdint.h>

#define SCREEN_WIDTH_PIXELS 1024
#define SCREEN_HEIGHT_PIXELS 768

typedef struct {
  int x;
  int y;
} Segment;

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} Pixel;

/**
 * @brief Imprime un string en pantalla.
 *
 * Si isErr es 1, se imprime en rojo; de lo contrario, se utiliza el color de
 * fuente actual.
 *
 * @param string El string a imprimir.
 * @param length La longitud del string.
 * @param isErr Indica si se debe imprimir en rojo (1) o en el color de fuente
 * (0).
 */
int print_str(char *string, int length, int isErr);

/**
 * @brief Establece el color de fuente.
 *
 * Esta función asigna un nuevo color a la fuente utilizando el valor
 * proporcionado.
 */
void set_color(char r, char g, char b);

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
