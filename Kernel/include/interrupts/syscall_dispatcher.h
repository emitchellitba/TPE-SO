#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <interrupts.h>
#include <keyboardDriver.h>
#include <lib.h>
#include <logger.h>
#include <soundDriver.h>
#include <stdarg.h>
#include <stdint.h>
#include <time_rtc.h>
#include <videodriver.h>

#define INVALID_SYS_ID -1

int64_t syscall_dispatcher(uint64_t rax, ...);

int64_t sys_read(va_list args);
int64_t sys_write(va_list args);
int64_t sys_clear_screen(va_list args);
int64_t sys_change_color(va_list args);
int64_t sys_draw_sqr(va_list args);
int64_t sys_free_draw(va_list args);
int64_t sys_get_time(va_list args);
int64_t sys_sleep(va_list args);
int64_t sys_zoom(va_list args);
int64_t sys_fill_out_buffer(va_list args);
int64_t sys_beep(va_list args);
int64_t sys_read_kmsg(va_list args);

#endif
