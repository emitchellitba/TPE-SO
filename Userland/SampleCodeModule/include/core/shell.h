#ifndef _SHELL_H_
#define _SHELL_H_

#include <core.h>
#include <display.h>
#include <info.h>
#include <libu.h>
#include <stdLibrary.h>

#define BUFF_SIZE 500
#define SPECIAL_KEY_MAX_VALUE 5

#define MAX_PARAMS 10
#define MAX_PARAM_LEN 32
#define MAX_CMD_LEN 32

typedef struct {
  char cmd[MAX_CMD_LEN];
  int param_count;
  char params[MAX_PARAMS][MAX_PARAM_LEN];
} parsed_input_t;

int shell_main(int argc, char *argv[]);
int get_total_commands();

#endif // _SHELL_H_
