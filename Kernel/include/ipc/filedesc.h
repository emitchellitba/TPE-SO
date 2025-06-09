#ifndef _FD_H_
#define _FD_H_

#include <pcb.h>
#include <sys/types.h>

void set_default_fds(proc_t *proc);
int inherit_fds(proc_t *target, proc_t *source, int fds[]);

#endif // _FD_H_
