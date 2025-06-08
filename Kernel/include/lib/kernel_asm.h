#ifndef _KERNEL_ASM_H
#define _KERNEL_ASM_H

extern void call_timer_tick();
extern void _hlt();
extern void do_exit(int code);

#endif // _KERNEL_ASM_H
