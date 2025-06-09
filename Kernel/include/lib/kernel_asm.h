#ifndef _KERNEL_ASM_H
#define _KERNEL_ASM_H

extern void call_timer_tick();
extern void _hlt();
extern void acquire(uint8_t *lock);
extern void release(uint8_t *lock);

#endif // _KERNEL_ASM_H
