#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../include/ds/queue.h"
#include <logger.h>
#include <proc/process.h>

typedef struct scheduler_cdt *scheduler_t;

void initialize_scheduler();
void proc_ready(struct proc *p);
void schedule();
int process_wrapper();
uint64_t scheduler_handler(uint64_t last_rsp);

#endif
