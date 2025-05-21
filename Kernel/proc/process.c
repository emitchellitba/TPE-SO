#include "../include/lib/memory_manager.h"
#include <ds/queue.h>
#include <lib/logger.h>
#include <proc/process.h>

static int proc_log_level = LOG_DEBUG;
LOGGER_DEFINE(proc, proc_log, proc_log_level)

/**
 * Manejo de PIDS: Por ahora podemos simplemente usar un int con el ultimo pid
 * utilizado y cada nuevo proceso le asignamos el pid siguiente. En el futuro,
 * podemos usar una estructura como un bitmap para asignar los pids.
 */

struct queue *procs = QUEUE_NEW();
pid_t last_pid = 0;

pid_t proc_pid_alloc() { return last_pid++; }

int proc_new(proc_t **ref, char *name, proc_t *parent, uint64_t entry,
             char **argv, uint64_t argc) {
  pid_t new_pid = proc_pid_alloc();

  proc_t *proc = (proc_t *)kmalloc(kernel_mem, sizeof(proc_t));
  if (!proc) {
    proc_log(LOG_ERR, "Error allocating process\n");
    return -1;
  }

  /** Alloc stack mem for process */
  proc->stack_start = (uint64_t)kmalloc(kernel_mem, STACK_SIZE);
  if (!proc->stack_start) {
    proc_log(LOG_ERR, "Error allocating stack memory for process\n");
    kmm_free(proc, kernel_mem);
    return -1;
  }
  proc->stack_pointer = proc->stack_start + STACK_SIZE;

  memset(proc, 0, sizeof(proc_t));
  proc->pid = new_pid;
  proc->name = name;
  proc->status = READY;
  proc->parent = parent;
  proc->entry = (uint64_t)entry;

  /** Copy argv in kernel heap */

  char **argv_copy =
      (char **)kmalloc(kernel_mem, sizeof(char *) * argc /* + 1 */);
  if (!argv_copy) {
    proc_log(LOG_ERR, "Error allocating argv memory for process\n");
    kmm_free(proc->stack_start, kernel_mem);
    kmm_free(proc, kernel_mem);
    return -1;
  }

  /** TODO: COPY ARGS INTO ARGV */

  proc->argv = argv_copy;
  proc->argc = argc;

  struct queue wait_queue = {0};
  proc->wait_queue = wait_queue;

  proc->exit = -1;

  return 0;
}

struct proc *proc_pid_find(pid_t pid);

int proc_init(struct proc *proc);

void proc_kill(struct proc *proc);

/** Cuando un proceso termina se debe llamar a esta funcion que se encarga... */
int proc_reap(struct proc *proc);
