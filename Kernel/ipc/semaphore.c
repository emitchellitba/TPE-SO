#include <ipc/semaphore.h>

#include <process.h>
#include <scheduler.h>

// TODO: error handling (enum, data type, etc.)
// TODO: add process block/unblock functions in scheduler.c

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
    if (!semaphore_table[id].waiting_process_queue) {
      return NULL; // Error creating the waiting queue
    }
    semaphore_table[id].value = value;
    semaphore_table[id].in_use = 1;
    semaphore_table[id].waiting_process_queue = queue_new();
    semaphore_table[id].lock = 1;
    return &semaphore_table[id];
  } else {
    return NULL; // Invalid semaphore or already in use
  }
}

uint64_t my_sem_destroy(semaphore_t *sem) {
  if (sem_is_open(sem)) {
    sem->in_use = 0;
    queue_free(sem->waiting_process_queue); // Free the waiting queue
  } else {
    return -1; // Error: semaphore is not open or is invalid
  }
}

semaphore_t *my_sem_open(uint64_t id) {
  if (id >= MAX_SEMAPHORES || !semaphore_table[id].in_use) {
    return NULL; // Invalid semaphore or not in use
  }

  return &semaphore_table[id];
}

uint64_t my_sem_post(semaphore_t *sem) {
  if (!sem_is_open(sem)) {
    // printk_err("my_sem_post: Semaphore ID %lu is not open or is invalid.\n", sem ? sem->id : -1);
    return -1; // Error: invalid semaphore or not in use
  }

  acquire(&(sem->lock)); // Acquire lock to protect the semaphore

  proc_t *process_to_wake = (proc_t *)dequeue(sem->waiting_process_queue);

  if (process_to_wake) {
    // There is a process waiting in the semaphore queue.
    // We wake it up. The semaphore value is not incremented;
    // the 'post' is consumed by the process that wakes up.
    // printk_info("my_sem_post: Sem ID %lu, waking up process PID %d.\n", sem->id, process_to_wake->pid);

    // Free the lock BEFORE calling proc_ready, as proc_ready
    // could lead to a context switch if the woken process has high priority.
    release(&(sem->lock));

    proc_ready(
        process_to_wake); // Use the scheduler function to set it to READY
  } else {
    // No one waiting, just increment the semaphore value.
    sem->value++;
    // printk_info("my_sem_post: Sem ID %lu, nobody waiting. Value incremented to %lu.\n", sem->id, sem->value);
    release(&(sem->lock));
  }

  return 0; // Success
}

uint64_t my_sem_wait(semaphore_t *sem) {
  if (!sem_is_open(sem)) {
    return -1; // Error: semaphore not open or invalid
  }
  while (1) {
    acquire(&(sem->lock));
    if (sem->value > 0) {
      sem->value--;
      release(&(sem->lock));
      return 0; // Success
    } else {
      // Add current process to the waiting queue
      struct proc *current_proc = get_running();
      enqueue(sem->waiting_process_queue, current_proc);
      release(&(sem->lock));
      block_current(BLK_SEMAPHORE, sem); // Block current process
    }
  }
}

uint64_t my_sem_trywait(semaphore_t *sem) {
    if (!sem_is_open(sem)) {
        return (uint64_t)-1; // Error: semaphore not open
    }
    acquire(&(sem->lock));
    if (sem->value > 0) {
        sem->value--;
        release(&(sem->lock));
        return 0; // Success
    } else {
        release(&(sem->lock));
        return 1; // Would block (no value available)
    }
}

uint8_t sem_is_open(semaphore_t *sem) {
  if (sem && sem->in_use) {
    return 1; // Semaphore is open
  }
  return 0; // Semaphore is closed or invalid
}
