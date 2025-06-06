#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <exec.h>
#include <fs.h>
#include <interrupts.h>
#include <kernel_asm.h>
#include <keyboardDriver.h>
#include <lib.h>
#include <lib/error.h>
#include <logger.h>
#include <pipe.h>
#include <process.h>
#include <scheduler.h>
#include <soundDriver.h>
#include <stdarg.h>
#include <stdint.h>
#include <time_rtc.h>
#include <videoDriver.h>

#define INVALID_SYS_ID -1

#define DECLARE_SYSCALL(name) int64_t name(va_list args);

extern scheduler_t scheduler;

int64_t syscall_dispatcher(uint64_t rax, ...);

DECLARE_SYSCALL(sys_read)
DECLARE_SYSCALL(sys_write)
DECLARE_SYSCALL(sys_clear_screen)
DECLARE_SYSCALL(sys_change_color)
DECLARE_SYSCALL(sys_draw_sqr)
DECLARE_SYSCALL(sys_free_draw)
DECLARE_SYSCALL(sys_get_time)
DECLARE_SYSCALL(sys_sleep)
DECLARE_SYSCALL(sys_zoom)
DECLARE_SYSCALL(sys_fill_out_buffer)
DECLARE_SYSCALL(sys_beep)
DECLARE_SYSCALL(sys_read_kmsg)
DECLARE_SYSCALL(sys_pipe_create)
DECLARE_SYSCALL(sys_pipe_open)
DECLARE_SYSCALL(sys_pipe_close)
DECLARE_SYSCALL(sys_get_procs)
DECLARE_SYSCALL(sys_load_program)
DECLARE_SYSCALL(sys_rm_program)
DECLARE_SYSCALL(sys_get_programs)
DECLARE_SYSCALL(sys_spawn_process)
DECLARE_SYSCALL(sys_kill_proc)
DECLARE_SYSCALL(sys_change_priority)
DECLARE_SYSCALL(sys_exit)
DECLARE_SYSCALL(sys_block)
DECLARE_SYSCALL(sys_unblock)
DECLARE_SYSCALL(sys_copy_fd)
DECLARE_SYSCALL(sys_close_fd)

#endif
