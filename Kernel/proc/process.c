#include "process.h"
#include "../include/lib/memory_manager.h"
#include <ds/queue.h>
#include <lib/logger.h>
#include <proc/process.h>

static int proc_log_level = LOG_DEBUG;
LOGGER_DEFINE(proc, proc_log, proc_log_level)

#define MAX_PROCESSES 64
static struct proc *process_table[MAX_PROCESSES] = {NULL};
// static int process_count = 0;
// por cómo lo implementé, no es necesario un contador de procesos ya que
// los procesos hacen un first-fit en la tabla de procesos

pid_t proc_pid_alloc() { 
  for (pid_t i = 0; i < MAX_PROCESSES; i++) {
    if (process_table[i] == NULL) {
      return i; // Retorna el primer PID disponible
    }
  }
  proc_log(LOG_ERR, "No available PIDs\n");
  return -1; // No hay PIDs disponibles
}

/**
 * Se encarga de alocar la estructura basica del proceso
 *
 * Recibe un puntero a un puntero de proceso e informacion
 * sobre el proceso
 *
 * Retorna 0 si se pudo crear el proceso, 1 si hubo error
 */
int proc_new(proc_t **ref) {
  pid_t pid;
  if((pid = proc_pid_alloc()) < 0) {
    proc_log(LOG_ERR, "Error allocating PID for new process\n");
    return -NOMEMERR; // No hay PIDs disponibles
  }

  // Alocamos memoria para el proceso
  int err = 0;
  proc_t *proc = (proc_t *)kmalloc(kernel_mem, sizeof(proc_t));

  if (!proc) {
    proc_log(LOG_ERR, "Error allocating process\n");
    err = -NOMEMERR;
    return err;
  }

  process_table[pid] = proc;
  proc->pid = pid;
  proc_log(LOG_DEBUG, "Process created with PID %d\n", pid);

  if (ref) {
    *ref = proc;
  }

  return 0;
}

/**
 * Inicializa el stack y la informacion del proceso
 */
int proc_init(proc_t *proc, const char *name, proc_t *parent,
              proc_main_function entry) {
  int err = 0;

  if (!proc) {
    proc_log(LOG_ERR, "Error initializing process\n");
    err = -INVALARGSERR;
    return err;
  }

  proc->stack_start = (uint64_t *)kmalloc(kernel_mem, STACK_SIZE);
  if (!proc->stack_start) {
    proc_log(LOG_ERR, "Error allocating stack memory for process\n");
    err = -NOMEMERR;
    return err;
  }
  proc->stack_pointer = (uint64_t *)((char *)proc->stack_start + STACK_SIZE);

  proc->name = name;
  proc->parent = parent;
  proc->entry = entry;

  struct queue wait_queue = {0};
  proc->wait_queue = wait_queue;

  proc->status = READY;
  proc->exit = -1;

  return 0;
}

void proc_kill(struct proc *proc);

/** Cuando un proceso termina se debe llamar a esta funcion que se encarga... */
int proc_reap(struct proc *proc);
