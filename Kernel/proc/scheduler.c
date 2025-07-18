// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <scheduler.h>

#include <kernel_asm.h>
#include <logger.h>
#include <process.h>

typedef struct scheduler_cdt {
  int init;
  struct queue *ready_processes;
  struct queue *blocked_processes;
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

  scheduler->init = 1;
  scheduler->current_process = NULL;
}

void proc_ready(proc_t *p) {
  if (!scheduler || !scheduler->ready_processes || !p)
    return;

  p->status = READY;
  p->block_reason = BLK_NONE;
  p->waiting_on = scheduler->ready_processes;

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

  struct queue *ready_queue = scheduler->ready_processes;

  if (scheduler->current_process) {
    scheduler->current_process->stack_pointer = (uint64_t *)last_rsp;
  }

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
    return -1;
  }

  if (process->status == ZOMBIE || process->status == DEAD) {
    printk(
        "block: Intento de bloquear un proceso zombie o terminado (PID: %d).\n",
        process->pid);
    return -1;
  }

  if (process->status == BLOCKED) {
    printk("Advertencia: Proceso PID %d ya estaba bloqueado.\n", pid_to_block);
    return 0;
  }

  block(process, BLK_ARBITRARY, NULL);

  return 0;
}

int unblock_process_by_pid(pid_t pid_to_unblock) {
  proc_t *process = get_proc_by_pid(pid_to_unblock);

  if (process == NULL) {
    return -1;
  }

  if (process->status != BLOCKED) {
    printk("Advertencia: Proceso PID %d no estaba bloqueado (estado actual: "
           "%d).\n",
           pid_to_unblock, process->status);
    return 0;
  }

  proc_ready(process);

  return 0;
}

void sched_rm_current() { scheduler->current_process = NULL; }
