#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include <scheduler.h>
#include <process.h>

extern void aquire(uint8_t *lock);
extern void release(uint8_t *lock);

#define MAX_SEMAPHORES 1024

typedef struct {
    uint8_t lock;
    uint8_t in_use;
    uint64_t id;
    uint64_t value;
    struct queue * waiting_process_queue;
} semaphore_t;

#endif
