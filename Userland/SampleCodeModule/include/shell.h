#ifndef _SHELL_H_
#define _SHELL_H_

typedef enum {HELP = 0, DATE, ZOOM_IN, ZOOM_OUT, COLOR_FONT, COLOR_BG, CLEAR, EXIT, REGISTERS} command_id;

extern void opCodeExc();

void start_shell();
void get_input();
command_id process_input(char * input);
void execute_input(command_id command);
void help();
void print_global_date_time();
void print_local_date_time();
void command_not_found();
void exit();
void exception1();
void exception2();
void get_registers();

#endif