#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <lib.h>
#include <ringbuf.h>
#include <scheduler.h>
#include <stdint.h>

typedef enum { LEFT = 1, UP, DOWN, RIGHT } ARROWS;

void press_key();
extern uint8_t esc;
int load_buffer(char *buffer, size_t count);

#endif