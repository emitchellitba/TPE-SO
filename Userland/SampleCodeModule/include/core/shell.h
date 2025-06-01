#ifndef _SHELL_H_
#define _SHELL_H_

#include <core.h>
#include <display.h>
#include <info.h>
#include <libu.h>
#include <stdLibrary.h>

#define BUFF_SIZE 500
#define SPECIAL_KEY_MAX_VALUE 5

typedef enum {
  HELP = 0,
  DATE,
  ZOOM_IN,
  ZOOM_OUT,
  COLOR_FONT,
  COLOR_BG,
  CLEAR,
  EXIT,
  REGISTERS
} command_id;
// TODO: completar este enum o borrarlo si no es necesario

int shell_main(int argc, char *argv[]);
int get_total_commands();
void get_input(char *buffer);

#endif
