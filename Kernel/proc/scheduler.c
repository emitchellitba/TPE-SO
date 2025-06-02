
#include <scheduler.h>

extern void _hlt();
extern unsigned long ticks;

scheduler_t scheduler = NULL;
struct queue ready_queue = {0};

void initialize_scheduler() {
  scheduler = kmalloc(kernel_mem, sizeof(struct scheduler_cdt));
  if (!scheduler) {
    printk("CRITICAL: Failed to allocate memory for scheduler\n");
    return;
  }

  scheduler->ready_processes = &ready_queue;

  if (!scheduler->ready_processes) {
    printk("CRITICAL: Failed to create ready processes queue\n");
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

  enqueue(scheduler->ready_processes, p);

  p->status = READY;
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

  if (scheduler->current_died) {
    scheduler->current_died = 0;
    enqueue_next_process();
    return (uint64_t)scheduler->current_process->stack_pointer;
  }

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

void sched_current_died() { scheduler->current_died = 1; }

void sched_ready_queue_remove(struct proc *proc) {
  if (!scheduler || !scheduler->ready_processes || !proc)
    return;

  queue_remove(scheduler->ready_processes, proc);
}
