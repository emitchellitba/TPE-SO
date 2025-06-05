#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <sys/types.h>

typedef enum block_reason block_reason_t;
struct queue;
typedef struct proc proc_t;
typedef struct scheduler_cdt *scheduler_t;

void initialize_scheduler();
void proc_ready(proc_t *p);
uint64_t schedule(uint64_t last_rsp);
void enqueue_next_process();
int process_wrapper(uint64_t user_argc, char **user_argv);
void sched_current_died();
proc_t *get_running();
int block_process_by_pid(pid_t pid_to_block);
void block(proc_t *process, block_reason_t reason, void *waiting_resource);
void block_current(block_reason_t reason, void *waiting_resource);

#endif
