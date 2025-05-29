
#include <scheduler.h>

extern void _hlt();
extern unsigned long ticks;

typedef struct scheduler_cdt {
  int init;
  struct queue *ready_processes;
  char idle_flag;
  struct proc *current_process;
} scheduler_cdt;

scheduler_t scheduler = NULL;

uint64_t scheduler_handler(uint64_t last_rsp) {
  if (!scheduler || !scheduler->init) {
    return last_rsp;
  }

  if (!scheduler->idle_flag) {
    scheduler->current_process->stack_pointer = (uint64_t *)last_rsp;
    proc_ready(scheduler->current_process);
  }

  schedule();

  return (uint64_t)scheduler->current_process->stack_pointer;
}

void initialize_scheduler() {
  scheduler = kmalloc(kernel_mem, sizeof(struct scheduler_cdt));
  if (!scheduler) {
    printk("CRITICAL: Failed to allocate memory for scheduler\n");
    return;
  }
  scheduler->ready_processes = QUEUE_NEW();
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

  while (1) {
    _hlt();
  }
}

void schedule() {
  if (!scheduler || !scheduler->ready_processes)
    return;

  struct queue *ready_queue = scheduler->ready_processes;

  if (ready_queue->count == 0) {
    kernel_idle();
  } else {
    scheduler->current_process = (struct proc *)dequeue(ready_queue);
    if (scheduler->current_process) {
      scheduler->current_process->status = RUNNING;
      scheduler->idle_flag = 0;
    } else { /* No deberia pasar nunca pero por seguridad llama a idle */
      kernel_idle();
    }
  }
}

int process_wrapper() {
  proc_t *current_p = scheduler->current_process;

  // int user_argc;
  // char **user_argv;

  // // Assembly to pop/read argc and argv from [RSP] and [RSP+8]
  // // asm volatile(
  // //     "movq (%%rsp), %0\n\t" // Read argc from current stack pointer
  // //     "movq 8(%%rsp), %1"    // Read argv from current stack pointer + 8
  // //     : "=r"(user_argc), "=r"(user_argv)::"memory");

  // // Adjust stack pointer if values were read, not popped, or if more things
  // are on stack
  // // For simplicity, assume they are just read for now.

  // if (current_p && current_p->entry)
  // {
  //     int exit_code = current_p->entry(user_argc, user_argv);
  //     // Call a system call or kernel function to terminate the process
  //     // sys_exit(exit_code);
  //     printk("Process %s exited with %d\n", current_p->name, exit_code);
  // }
  // else
  // {
  //     printk("Error: No entry point or process for wrapper!\n");
  //     // sys_exit(-1);
  // }

  while (1) {
  }
}
