#include <semaphores/semaphore.h>

// TODO: manejo de errores (enum, tipo de dato, etc.)
// TODO: agregar fuciones de bloqueo/desbloqueo de procesos en scheduler.c

static semaphore_t semaphore_table[MAX_SEMAPHORES];

// Se inicializan todos los semáforos en la tabla con su id y estado inactivo
void my_sem_init() {
    for(int i = 0; i < MAX_SEMAPHORES; i++) {
        semaphore_table[i].id = i;
        semaphore_table[i].in_use = 0;
    }
}

// Retorna el semáforo con el id especificado, o NULL si no existe o está en uso
semaphore_t *my_sem_create(uint64_t id, uint64_t value) {
    if(id < MAX_SEMAPHORES && !semaphore_table[id].in_use) {
        if(!semaphore_table[id].waiting_process_queue) {
            return NULL; // Error al crear la cola de espera
        }
        semaphore_table[id].value = value;
        semaphore_table[id].in_use = 1;
        semaphore_table[id].waiting_process_queue = queue_new();
        semaphore_table[id].lock = 1;
        return &semaphore_table[id];
    } else {
        return NULL; // Semáforo no válido o ya en uso
    }
}

// Libera el semáforo pasado cómo parámetro
void my_sem_destroy(semaphore_t *sem) {
    if(sem_is_open(sem)) {
        sem->in_use = 0;
        queue_free(sem->waiting_process_queue); // Liberar la cola de espera
    }
}

// Retorna la referencia a un semáforo si existe y está en uso, o NULL si no
semaphore_t *my_sem_open(uint64_t id) {
    if(id >= MAX_SEMAPHORES || !semaphore_table[id].in_use) {
        return NULL; // Semáforo no válido o no está en uso
    }

    return &semaphore_table[id];
}

uint64_t my_sem_post(semaphore_t *sem) {
    if(!sem_is_open(sem)) {
        return -1; // Error: semáforo no está abierto
    }
    aquire(&(sem->lock));
    sem->value++;
    release(&(sem->lock));
    return unblock_sem_process((struct proc*)dequeue(sem->waiting_process_queue));
}

uint64_t my_sem_wait(semaphore_t *sem) {
    if(!sem_is_open(sem)) {
        return -1; // Error: semáforo no está abierto
    }
    while(1) {
        aquire(&(sem->lock));
        if(sem->value > 0) {
            sem->value--;
            release(&(sem->lock));
            return 0; // Éxito
        } else {
            // Agregar el proceso actual a la cola de espera
            struct proc *current_proc = get_running_process();
            enqueue(sem->waiting_process_queue, current_proc);
            release(&(sem->lock));
            block_curent_process(); // Bloquear el proceso actual
        }   
    }
}

uint8_t sem_is_open(semaphore_t *sem) {
    if(sem && sem->in_use) {
        return 1; // El semáforo está abierto
    }
    return 0; // El semáforo no está abierto
}
