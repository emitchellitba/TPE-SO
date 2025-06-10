#include <ipc/semaphore.h>

#include <process.h>
#include <scheduler.h>

// Helper macro to get the semaphore pointer from id
#define SEM_PTR(id) (&semaphore_table[(id)])

// TODO: error handling (enum, data type, etc.)
// TODO: add process block/unblock functions in scheduler.c

static semaphore_t semaphore_table[MAX_SEMAPHORES];

void my_sem_init() {
    for (int i = 0; i < MAX_SEMAPHORES; i++) {
        semaphore_table[i].id = i;
        semaphore_table[i].in_use = 0;
    }
}

semaphore_t* my_ksem_init(uint64_t id, uint64_t value) {
    if (id >= MAX_SEMAPHORES) return -1;
    semaphore_t *sem = SEM_PTR(id);
    sem->id = id;
    sem->value = value;
    sem->in_use = 1;
    sem->waiting_process_queue = queue_new();
    if (!sem->waiting_process_queue)
        return -1;
    return 0;
}

uint64_t my_sem_create(uint64_t id, uint64_t value) {
    if (id < MAX_SEMAPHORES && !semaphore_table[id].in_use) {
        semaphore_table[id].waiting_process_queue = queue_new();
        if (!semaphore_table[id].waiting_process_queue) {
            return -1; // Error creating the waiting queue
        }
        semaphore_table[id].value = value;
        semaphore_table[id].in_use = 1;
        semaphore_table[id].lock = 1;
        return 0; // Success
    } else {
        return -1; // Invalid semaphore or already in use
    }
}

uint64_t my_sem_destroy(uint64_t id) {
    if (id >= MAX_SEMAPHORES) return -1;
    semaphore_t *sem = SEM_PTR(id);
    if (sem_is_open(id)) {
        sem->in_use = 0;
        queue_free(sem->waiting_process_queue); // Free the waiting queue
        return 0;
    } else {
        return -1; // Error: semaphore is not open or is invalid
    }
}

uint64_t my_sem_open(uint64_t id) {
    if (id >= MAX_SEMAPHORES || !semaphore_table[id].in_use) {
        return -1; // Invalid semaphore or not in use
    }
    return 0; // Success
}

uint64_t my_sem_post(uint64_t id) {
    if (!sem_is_open(id)) {
        return -1; // Error: invalid semaphore or not in use
    }
    semaphore_t *sem = SEM_PTR(id);
    acquire(&(sem->lock)); // Acquire lock to protect the semaphore

    proc_t *process_to_wake = (proc_t *)dequeue(sem->waiting_process_queue);

    if (process_to_wake) {
        release(&(sem->lock));
        proc_ready(process_to_wake); // Set process to READY
    } else {
        sem->value++;
        release(&(sem->lock));
    }
    return 0; // Success
}

uint64_t my_sem_wait(uint64_t id) {
    if (!sem_is_open(id)) {
        return -1; // Error: semaphore not open or invalid
    }
    semaphore_t *sem = SEM_PTR(id);
    acquire(&(sem->lock));
    if (sem->value > 0) {
        sem->value--;
        release(&(sem->lock));
        return 0; // Success
    } else {
        struct proc *current_proc = get_running();
        enqueue(sem->waiting_process_queue, current_proc);
        release(&(sem->lock));
        block_current(BLK_SEMAPHORE, sem); // Block current process
        // When unblocked, return 0
        return 0;
    }
}

uint64_t my_sem_trywait(uint64_t id) {
    if (!sem_is_open(id)) {
        return -1; // Error: semaphore not open
    }
    semaphore_t *sem = SEM_PTR(id);
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

uint8_t sem_is_open(uint64_t id) {
    if (id < MAX_SEMAPHORES && semaphore_table[id].in_use) {
        return 1; // Semaphore is open
    }
    return 0; // Semaphore is closed or invalid
}
