#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../include/ds/queue.h"
#include <logger.h>
#include <proc/process.h>

typedef struct scheduler_cdt *scheduler_t;

typedef struct scheduler_cdt {
  int init;
  struct queue *ready_processes;
  char idle_flag;
  char current_died;
  struct proc *current_process;
} scheduler_cdt;

void initialize_scheduler();
void proc_ready(struct proc *p);
uint64_t schedule(uint64_t last_rsp);
void enqueue_next_process();
int process_wrapper(uint64_t user_argc, char **user_argv);
void sched_current_died();
void sched_ready_queue_remove(struct proc *proc);

#endif
