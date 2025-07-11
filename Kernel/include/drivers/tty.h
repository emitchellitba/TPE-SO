#ifndef _TTY_H_
#define _TTY_H_

#include <pcb.h>

int read_from_keyboard(char *buffer, size_t count);
void set_foreground_process(proc_t *proc, proc_t *caller);
void reset_foreground_process();
void signal_kill_tty();

#endif // _TTY_H_
