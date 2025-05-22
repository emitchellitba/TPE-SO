#include "../include/proc/scheduler.h"
#include "../include/ds/queue.h"

typedef struct scheduler_cdt {
    struct queue * ready_processes;

} scheduler_cdt;

scheduler_t scheduler;

void initialize_scheduler() {
    scheduler->ready_processes = QUEUE_NEW();
}

void* schedule(void* last_rsp) {
    
}

