#include <ipc/semaphore.h>

#include <process.h>
#include <scheduler.h>

// TODO: manejo de errores (enum, tipo de dato, etc.)
// TODO: agregar fuciones de bloqueo/desbloqueo de procesos en scheduler.c

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

uint64_t my_sem_destroy(semaphore_t *sem) {
  if (sem_is_open(sem)) {
    sem->in_use = 0;
    queue_free(sem->waiting_process_queue); // Liberar la cola de espera
  } else {
    return -1; // Error: semáforo no está abierto o es inválido
  }
}

semaphore_t *my_sem_open(uint64_t id) {
  if (id >= MAX_SEMAPHORES || !semaphore_table[id].in_use) {
    return NULL; // Semáforo no válido o no está en uso
  }

  return &semaphore_table[id];
}

uint64_t my_sem_post(semaphore_t *sem) {
  if (!sem_is_open(sem)) {
    // printk_err("my_sem_post: Semáforo ID %lu no está abierto o es
    // inválido.\n", sem ? sem->id : -1);
    return -1; // Error: semáforo no válido o no está en uso
  }

  acquire(&(sem->lock)); // Adquirir lock para proteger el semáforo

  proc_t *process_to_wake = (proc_t *)dequeue(sem->waiting_process_queue);

  if (process_to_wake) {
    // Hay un proceso esperando en la cola del semáforo.
    // Lo despertamos. El valor del semáforo no se incrementa;
    // el 'post' es consumido por el proceso que se despierta.
    // printk_info("my_sem_post: Sem ID %lu, despertando proceso PID %d.\n",
    // sem->id, process_to_wake->pid);

    // Liberar el lock ANTES de llamar a proc_ready, ya que proc_ready
    // podría llevar a un cambio de contexto si el proceso despertado tiene alta
    // prioridad.
    release(&(sem->lock));

    proc_ready(
        process_to_wake); // Usar la función del scheduler para ponerlo en READY
  } else {
    // Nadie esperando, simplemente incrementamos el valor del semáforo.
    sem->value++;
    // printk_info("my_sem_post: Sem ID %lu, nadie esperando. Valor incrementado
    // a %lu.\n", sem->id, sem->value);
    release(&(sem->lock));
  }

  return 0; // Éxito
}

uint64_t my_sem_wait(semaphore_t *sem) {
  if (!sem_is_open(sem)) {
    return -1; // Error: semáforo no está abierto
  }
  while (1) {
    acquire(&(sem->lock));
    if (sem->value > 0) {
      sem->value--;
      release(&(sem->lock));
      return 0; // Éxito
    } else {
      // Agregar el proceso actual a la cola de espera
      struct proc *current_proc = get_running();
      enqueue(sem->waiting_process_queue, current_proc);
      release(&(sem->lock));
      block_current(BLK_SEMAPHORE, sem); // Bloquear el proceso actual
    }
  }
}

uint8_t sem_is_open(semaphore_t *sem) {
  if (sem && sem->in_use) {
    return 1; // El semáforo está abierto
  }
  return 0; // El semáforo no está abierto
}
