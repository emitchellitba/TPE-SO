#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../include/ds/queue.h"
#include <logger.h>
#include <proc/process.h>

typedef struct scheduler_cdt *scheduler_t;

void initialize_scheduler();
void proc_ready(proc_t *p);
uint64_t schedule(uint64_t last_rsp);
int process_wrapper();
proc_t *get_running();
void block(proc_t *process, block_reason_t reason, void *waiting_resource);
void block_current(block_reason_t reason, void *waiting_resource);

#endif
