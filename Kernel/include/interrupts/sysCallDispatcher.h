#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <lib.h>
#include <stdint.h>

typedef enum { STDIN = 0, STDOUT, STDERR } FDS;

void syscallDispatcher(uint64_t rax, ...);
size_t sys_write(FDS fd, const char *buf, size_t count);
size_t sys_read(FDS fd, char *buf, size_t count);
size_t sys_clear_screen();
size_t sys_change_color(uint8_t background);

#endif