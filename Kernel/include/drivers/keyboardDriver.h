#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stddef.h>
#include <stdint.h>

extern uint8_t esc;

void handle_key_press();
int read_line(char *buffer, size_t count);

#endif
