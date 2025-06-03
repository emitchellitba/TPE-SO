#ifndef _INFO_H_
#define _INFO_H_

#include <libu.h>
#include <stdLibrary.h>
#include <stdint.h>

#define KMSG_BUFF_SIZE 1024

extern int get_total_commands();
extern uint64_t get_regist(uint64_t *regs);

int get_registers_cmd();
int help_cmd();
int print_global_date_time();
int print_local_date_time_cmd();
int show_kmsg_cmd();
int show_processes_cmd();
int show_programs_cmd();

#endif // _INFO_H_
