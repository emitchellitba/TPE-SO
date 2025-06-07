#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

extern void screen_clear();
extern void change_color(uint8_t background);
extern void zoom(uint8_t zoom_in);

int clear_cmd();
int zoom_cmd(int argc, char **argv);
int color_cmd(int argc, char **argv);
int cat_cmd(void);

#endif // _DISPLAY_H_
