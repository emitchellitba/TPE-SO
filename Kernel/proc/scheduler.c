
#include <scheduler.h>

extern void _hlt();
extern unsigned long ticks;

typedef struct scheduler_cdt {
  int init;
  struct queue *ready_processes;
  struct queue *blocked_processes;
  char idle_flag;
  struct proc *current_process;
} scheduler_cdt;

scheduler_t scheduler = NULL;
struct queue ready_queue = {0};
struct queue blocked_queue = {0};

void initialize_scheduler() {
  scheduler = kmalloc(kernel_mem, sizeof(struct scheduler_cdt));
  if (!scheduler) {
    printk("CRITICAL: Failed to allocate memory for scheduler\n");
    return;
  }

  scheduler->ready_processes = &ready_queue;
  scheduler->blocked_processes = &blocked_queue;

  if (!scheduler->ready_processes) {
    printk("CRITICAL: Failed to create ready processes queue\n");
    kmm_free(scheduler, kernel_mem);
    scheduler = NULL;
    return;
  }
  if (!scheduler->blocked_processes) {
    printk("CRITICAL: Failed to create blocked processes queue\n");
    kmm_free(scheduler, kernel_mem);
    scheduler = NULL;
    return;
  }

  scheduler->init = 1;
  scheduler->idle_flag = 1;
  scheduler->current_process = NULL;
}

void proc_ready(struct proc *p) {
  if (!scheduler || !scheduler->ready_processes || !p)
    return;

  p->status = READY;
  p->block_reason = BLK_NONE; // Limpiar la razon de bloqueo
  p->waiting_on = NULL;       // Limpiar el objeto de espera

  enqueue(scheduler->ready_processes, p);
  queue_node_remove(scheduler->blocked_processes, p);
}

static void kernel_idle() {
  scheduler->idle_flag = 1;

  // Chequeo si hay procesos listos
  while (scheduler->ready_processes->count == 0) {
    _hlt();
  }
}

// Encolar el siguiente proceso listo de manera segura
void enqueue_next_process() {
  scheduler->current_process =
      (struct proc *)dequeue(scheduler->ready_processes);
  if (scheduler->current_process) {
    scheduler->current_process->status = RUNNING;
    scheduler->current_process->has_quantum =
        scheduler->current_process->priority;
    scheduler->idle_flag = 0;
  } else { /* No deberia pasar nunca pero por seguridad llama a idle */
    printk("CRITICAL: ready_processes-> count mismanaged\n");
    kernel_idle();
  }
}

uint64_t schedule(uint64_t last_rsp) {
  if (!scheduler || !scheduler->init || !scheduler->ready_processes)
    return last_rsp;

  scheduler->current_process->stack_pointer = (uint64_t *)last_rsp;
  struct queue *ready_queue = scheduler->ready_processes;

  if (ready_queue->count == 0) {
    if (!scheduler->current_process) {
      kernel_idle();
    } else {
      scheduler->current_process->has_quantum =
          scheduler->current_process->priority;
    }
  } else {
    if (!scheduler->current_process) {
      enqueue_next_process();
    } else {
      if (!(--scheduler->current_process->has_quantum)) {
        scheduler->current_process->status = READY;
        proc_ready(scheduler->current_process);

        enqueue_next_process();
      }
    }
  }
  return (uint64_t)scheduler->current_process->stack_pointer;
}

int process_wrapper(uint64_t user_argc, char **user_argv) {
  proc_t *current_p = scheduler->current_process;

  if (current_p && current_p->entry) {
    current_p->entry(user_argc, user_argv);
  } else {
    printk("Error: No entry point or process for wrapper!\n");
    // sys_exit(-1);
  }

  while (1) // No se deberia llegar a este punto pues un sys_exit debe ocurrir
            // antes
  {
  }
}

uint64_t change_priority(pid_t pid, priority_t new_priority) {
  if (!scheduler || !scheduler->current_process)
    return 1;

  scheduler->current_process->priority = new_priority;
  return 0;
}

void yield() {
  scheduler->current_process->has_quantum = 0; // Forzar cambio de proceso
  call_timer_tick(); // Llamar al tick del timer para que se ejecute el
                     // scheduler
}

proc_t *get_running() {
  if (!scheduler || !scheduler->current_process) {
    return NULL; // No hay proceso corriendo
  }
  return scheduler->current_process;
}

void block(proc_t *process, block_reason_t reason, void *waiting_resource) {

  if (process == NULL) {
    return;
  }

  process->status = BLOCKED;
  process->block_reason = reason;
  process->waiting_on = waiting_resource;

  // Sacar el proceso de la cola de listos si está presente
  queue_node_remove(scheduler->ready_processes, process);
  // Mover el proceso a la cola de bloqueados
  enqueue(scheduler->blocked_processes, process);

  // Si el proceso se bloquea a sí mismo (es el proceso actual), debe ceder la
  // CPU.
  if (scheduler->current_process &&
      scheduler->current_process->pid == process->pid) {
    yield();
  }
}

void block_current(block_reason_t reason, void *waiting_resource) {
  proc_t *current = get_running();
  if (current) {
    block(current, reason, waiting_resource);
  } else {
    printk("block_current: No hay proceso corriendo para bloquear.\n");
  }
}

int block_process_by_pid(pid_t pid_to_block) {
  proc_t *process = get_proc_by_pid(pid_to_block);

  if (process == NULL) {
    return -1; // Error: Proceso no encontrado
  }

  if (process->status == ZOMBIE || process->status == DEAD) {
    printk(
        "block: Intento de bloquear un proceso zombie o terminado (PID: %d).\n",
        process->pid);
    return;
  }

  if (process->status == BLOCKED) {
    printk("Advertencia: Proceso PID %d ya estaba bloqueado.\n", pid_to_block);
    return 0; // O un código de error/advertencia específico
  }

  block(process, BLK_ARBITRARY, NULL); // Usar una razón genérica para bloqueo

  return 0;
}

int unblock_process_by_pid(pid_t pid_to_unblock) {
  proc_t *process = get_process_by_pid(pid_to_unblock);

  if (process == NULL) {
    return -1; // Error: Proceso no encontrado
  }

  if (process->status != BLOCKED) {
    // No estaba bloqueado, o está en un estado desde el cual no debería ser
    // "desbloqueado" directamente.
    printk("Advertencia: Proceso PID %d no estaba bloqueado (estado actual: "
           "%d).\n",
           pid_to_unblock, process->status);
    return 0; // O un código de error/advertencia específico
  }
  if (process->status == ZOMBIE || process->status == DEAD) {
    printk("Error: No se puede desbloquear un proceso zombie o terminado (PID: "
           "%d).\n",
           pid_to_unblock);
    return -2; // Error: Proceso zombie o terminado
  }

  proc_ready(
      process); // Cambiar el estado a READY y moverlo a la cola de listos

  return 0;
}
