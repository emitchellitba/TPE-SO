#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>

extern void aquire(uint8_t *lock);
extern void release(uint8_t *lock);

#define MAX_SEMAPHORES 1024

typedef struct {
  uint8_t lock;
  uint8_t in_use;
  uint64_t id;
  uint64_t value;
  struct queue *waiting_process_queue;
} semaphore_t;

void my_sem_init();
semaphore_t *my_sem_create(uint64_t id, uint64_t value);
void my_sem_destroy(semaphore_t *sem);
semaphore_t *my_sem_open(uint64_t id);
uint64_t my_sem_post(semaphore_t *sem);
uint64_t my_sem_wait(semaphore_t *sem);
uint8_t sem_is_open(semaphore_t *sem);

#endif
