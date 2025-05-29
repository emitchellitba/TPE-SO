#include "../include/lib/memory_manager.h"
#include <ds/queue.h>
#include <lib/logger.h>
#include <proc/process.h>

static int proc_log_level = LOG_DEBUG;
LOGGER_DEFINE(proc, proc_log, proc_log_level)

struct queue *procs = QUEUE_NEW();
pid_t last_pid = 0;

pid_t proc_pid_alloc() { return last_pid++; }

/**
 * Se encarga de alocar la estructura basica del proceso
 *
 * Recibe un puntero a un puntero de proceso e informacion
 * sobre el proceso
 *
 * Retorna 0 si se pudo crear el proceso, 1 si hubo error
 */
int proc_new(proc_t **ref) {
  int err = 0;
  proc_t *proc = (proc_t *)kmalloc(kernel_mem, sizeof(proc_t));

  if (!proc) {
    proc_log(LOG_ERR, "Error allocating process\n");
    err = -NOMEMERR;
    return err;
  }

  if (ref) {
    *ref = proc;
  }

  return 0;
}

/**
 * Inicializa el stack y la informacion del proceso
 */
int proc_init(proc_t *proc, const char *name, proc_t *parent) {
  int err = 0;

  if (!proc) {
    proc_log(LOG_ERR, "Error initializing process\n");
    err = -INVALARGSERR;
    return err;
  }

  proc->pid = proc_pid_alloc();

  proc->stack_start = (uint64_t *)kmalloc(kernel_mem, STACK_SIZE);
  if (!proc->stack_start) {
    proc_log(LOG_ERR, "Error allocating stack memory for process\n");
    err = -NOMEMERR;
    return err;
  }
  proc->stack_pointer = proc->stack_start + STACK_SIZE;

  proc->name = name;
  proc->parent = parent;

  struct queue wait_queue = {0};
  proc->wait_queue = wait_queue;

  proc->status = READY;
  proc->exit = -1;

  return 0;
}

void proc_kill(struct proc *proc);

/** Cuando un proceso termina se debe llamar a esta funcion que se encarga... */
int proc_reap(struct proc *proc);
