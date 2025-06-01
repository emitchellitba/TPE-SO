#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

extern void screen_clear();
extern void change_color(uint8_t background);
extern void zoom(uint8_t zoom_in);

int clear_cmd();
int zoom_in_cmd();
int zoom_out_cmd();
int change_font_color_cmd();
int change_bg_color_cmd();

#endif // _DISPLAY_H_
