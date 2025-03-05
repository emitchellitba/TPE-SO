#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <lib.h>

typedef enum {
    LEFT = 1, UP, DOWN, RIGHT
} ARROWS;

void press_key();
unsigned char getLastKey();
extern uint8_t esc;
void load_buffer(char * buffer, size_t count);

#endif