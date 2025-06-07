
#include <scheduler.h>

#include <kernel_asm.h>
#include <logger.h>
#include <process.h>

typedef struct scheduler_cdt {
  int init;
  struct queue *ready_processes;
  struct queue *blocked_processes;
  char current_died;
  proc_t *current_process;
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
  scheduler->current_process = NULL;
}

void proc_ready(proc_t *p) {
  if (!scheduler || !scheduler->ready_processes || !p)
    return;

  p->status = READY;
  p->block_reason = BLK_NONE; // Limpiar la razon de bloqueo
  p->waiting_on = NULL;       // Limpiar el objeto de espera

  enqueue(scheduler->ready_processes, p);
  queue_remove(scheduler->blocked_processes, p);
}

// Encolar el siguiente proceso listo de manera segura
void enqueue_next_process() {
  scheduler->current_process = (proc_t *)dequeue(scheduler->ready_processes);
  if (scheduler->current_process) {
    scheduler->current_process->status = RUNNING;
    scheduler->current_process->has_quantum =
        scheduler->current_process->priority;
  } else {
    printk("CRITICAL: No hay procesos listos para ejecutar.\n");
    while (1)
      ;
  }
}

uint64_t schedule(uint64_t last_rsp) {
  if (!scheduler || !scheduler->init || !scheduler->ready_processes)
    return last_rsp;

  if (scheduler->current_died) {
    scheduler->current_died = 0;
    enqueue_next_process();
    return (uint64_t)scheduler->current_process->stack_pointer;
  }

  scheduler->current_process->stack_pointer = (uint64_t *)last_rsp;
  struct queue *ready_queue = scheduler->ready_processes;

  if (ready_queue->count == 0) {
    if (!scheduler->current_process ||
        scheduler->current_process->status != RUNNING) {
      printk("PANIC: No hay procesos listos ni corriendo. \n");
      while (1)
        ;
    } else {
      scheduler->current_process->has_quantum =
          scheduler->current_process->priority;
    }
  } else {
    if (!scheduler->current_process) {
      enqueue_next_process();
    } else {
      if (scheduler->current_process->has_quantum >
          0) /* Validacion por seguridad */
        scheduler->current_process->has_quantum--;

      if (scheduler->current_process->has_quantum == 0) {
        if (scheduler->current_process->status != BLOCKED &&
            scheduler->current_process->status != ZOMBIE &&
            scheduler->current_process->status != DEAD) {
          scheduler->current_process->status = READY;
          proc_ready(scheduler->current_process);
        }

        enqueue_next_process();
      }
    }
  }

  return (uint64_t)scheduler->current_process->stack_pointer;
}

void yield() {
  scheduler->current_process->has_quantum = 0;
  call_timer_tick();
}

void sched_current_died() { scheduler->current_died = 1; }

void sched_ready_queue_remove(proc_t *proc) {
  if (!scheduler || !scheduler->ready_processes || !proc)
    return;

  queue_remove(scheduler->ready_processes, proc);
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

  queue_remove(scheduler->ready_processes, process);
  enqueue(scheduler->blocked_processes, process);

  if (scheduler->current_process &&
      scheduler->current_process->pid ==
          process->pid) { /* El proceso se bloqueo a si mismo, por lo tanto esta
                             corriendo */
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
    return -1;
  }

  if (process->status == BLOCKED) {
    printk("Advertencia: Proceso PID %d ya estaba bloqueado.\n", pid_to_block);
    return 0; // O un código de error/advertencia específico
  }

  block(process, BLK_ARBITRARY, NULL); // Usar una razón genérica para bloqueo

  return 0;
}

int unblock_process_by_pid(pid_t pid_to_unblock) {
  proc_t *process = get_proc_by_pid(pid_to_unblock);

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
