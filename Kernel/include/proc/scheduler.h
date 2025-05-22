#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef struct scheduler_cdt * scheduler_t; 

void initialize_scheduler();
void* schedule(void* last_rps);

#endif