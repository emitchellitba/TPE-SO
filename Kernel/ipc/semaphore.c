// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <ipc/semaphore.h>

#include <process.h>
#include <scheduler.h>

static semaphore_t semaphore_table[MAX_SEMAPHORES];

void my_sem_init() {
  for (int i = 0; i < MAX_SEMAPHORES; i++) {
    semaphore_table[i].id = i;
    semaphore_table[i].in_use = 0;
  }
}

int my_ksem_init(semaphore_t *sem, uint64_t value) {
  sem->id = MAX_SEMAPHORES;
  sem->value = value;
  sem->in_use = 1;
  sem->waiting_process_queue = queue_new();
  if (!sem->waiting_process_queue)
    return -1;
  return 0;
}

semaphore_t *my_sem_create(uint64_t id, uint64_t value) {
  if (id < MAX_SEMAPHORES && !semaphore_table[id].in_use) {
    semaphore_table[id].waiting_process_queue = queue_new();
    if (!semaphore_table[id].waiting_process_queue) {
      return NULL;
    }
    semaphore_table[id].value = value;
    semaphore_table[id].in_use = 1;
    semaphore_table[id].lock = 1;
    return &semaphore_table[id];
  } else {
    return NULL;
  }
}

uint64_t my_sem_destroy(semaphore_t *sem) {
  if (sem_is_open(sem)) {
    sem->in_use = 0;
    queue_free(sem->waiting_process_queue);
  } else {
    return -1;
  }

  return 0;
}

semaphore_t *my_sem_open(uint64_t id) {
  if (id >= MAX_SEMAPHORES || !semaphore_table[id].in_use) {
    return NULL;
  }

  return &semaphore_table[id];
}

uint64_t my_sem_post(semaphore_t *sem) {
  if (!sem_is_open(sem)) {

    return -1;
  }

  acquire(&(sem->lock));

  proc_t *process_to_wake = (proc_t *)dequeue(sem->waiting_process_queue);

  if (process_to_wake) {

    release(&(sem->lock));

    proc_ready(process_to_wake);
  } else {

    sem->value++;

    release(&(sem->lock));
  }

  return 0;
}

uint64_t my_sem_wait(semaphore_t *sem) {
  if (!sem_is_open(sem)) {
    return -1;
  }

  acquire(&(sem->lock));

  if (sem->value > 0) {
    sem->value--;
    release(&(sem->lock));
    return 0;
  } else {
    struct proc *current_proc = get_running();
    enqueue(sem->waiting_process_queue, current_proc);
    release(&(sem->lock));
    block_current(BLK_SEMAPHORE, sem);
  }

  return 0;
}

uint64_t my_sem_trywait(semaphore_t *sem) {
  if (!sem_is_open(sem)) {
    return (uint64_t)-1;
  }
  acquire(&(sem->lock));
  if (sem->value > 0) {
    sem->value--;
    release(&(sem->lock));
    return 0;
  } else {
    release(&(sem->lock));
    return 1;
  }
}

uint8_t sem_is_open(semaphore_t *sem) {
  if (sem && sem->in_use) {
    return 1;
  }
  return 0;
}
