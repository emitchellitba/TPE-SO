#include <process.h>

#include "../include/lib/memory_manager.h"
#include <ds/queue.h>
#include <lib/logger.h>

static int proc_log_level = LOG_DEBUG;
LOGGER_DEFINE(proc, proc_log, proc_log_level)

#define MAX_PROCESSES 64
static struct proc *process_table[MAX_PROCESSES] = {NULL};
static int process_count = 0;

static pid_t proc_pid_alloc() {
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

  if (process_count >= MAX_PROCESSES) {
    proc_log(LOG_ERR, "Maximum number of processes reached\n");
    return NOMEMERR;
  }

  int err = 0;
  proc_t *proc = (proc_t *)kmalloc(kernel_mem, sizeof(proc_t));
  if (!proc) {
    proc_log(LOG_ERR, "Error allocating process\n");
    err = NOMEMERR;
    return err;
  }

  memset(proc, 0, sizeof(proc_t));

  if (ref) {
    *ref = proc;
  }

  return 0;
}

/**
 * Inicializa el stack, fds y la informacion del proceso
 */
int proc_init(proc_t *proc, const char *name, proc_t *parent,
              proc_main_function entry) {
  int err = 0;

  if (!proc) {
    proc_log(LOG_ERR, "Error initializing process\n");
    err = -INVALARGSERR;
    return err;
  }

  proc->pid = proc_pid_alloc();
  process_table[proc->pid] = proc;

  proc->stack_start = (uint64_t *)kmalloc(kernel_mem, STACK_SIZE);
  if (!proc->stack_start) {
    proc_log(LOG_ERR, "Error allocating stack memory for process\n");
    err = -1;
    return err;
  }
  proc->stack_pointer = (uint64_t *)((char *)proc->stack_start + STACK_SIZE);

  proc->fds[0] = (fd_entry_t){
      .resource = NULL,
      .ops = &keyboard_ops,
      .type = FD_TERMINAL,
  };

  proc->fds[1] =
      (fd_entry_t){.resource = NULL, .ops = &video_ops, .type = FD_TERMINAL};

  proc->fds[2] = (fd_entry_t){
      .resource = NULL, .ops = &video_err_ops, .type = FD_TERMINAL};

  proc->name = name;
  proc->parent = parent;
  proc->entry = entry;

  // struct queue wait_queue = {0};
  // proc->wait_queue = wait_queue;

  proc->priority = QUANTUM_DEFAULT;

  proc->status = READY;
  proc->exit = -1;

  proc->block_reason = BLK_NONE;
  proc->waiting_on = NULL;

  return 0;
}

/**
 * Mata un proceso. Cambia su estado a ZOMBIE y libera sus recursos del kernel.
 */
void proc_kill(struct proc *proc) {
  if (!proc) {
    proc_log(LOG_ERR, "Cannot kill a NULL process\n");
    return;
  }

  if (proc->pid == 1) {
    proc_log(LOG_ERR, "Cannot kill the init process (PID: %d)\n", proc->pid);
    return;
  }

  proc_log(LOG_INFO, "Killing process %s (PID: %d)\n", proc->name, proc->pid);

  /* Eliminarlo del flujo del scheduler */
  if (proc->status == RUNNING) {
    sched_current_died();
  } else if (proc->status == BLOCKED) {
    /* TODO: Terminar esto */
  } else if (proc->status == READY) {
    sched_ready_queue_remove(proc);
  }

  /* Cambiar estado del proceso */
  proc->status = ZOMBIE;

  /* Cerrar pipes o semaforos */
  /* TODO: Terminar esto */

  /* Liberar recursos del kernel utilizados por el proceso (stack) */
  kmm_free(proc->stack_start, kernel_mem);

  /* Marcar todos los hijos como huerfanos */
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (process_table[i] && process_table[i]->parent == proc) {
      /* TODO: Reparentar a los hijos para que los limpie init */
      process_table[i]->parent = NULL;
      process_table[i]->status = READY;
    }
  }

  /* Liberar el nombre */
  kmm_free((void *)proc->name, kernel_mem);
  proc->name = NULL;

  /* Despertar al padre si lo hay y esta esperando al hijo */
  if (proc->parent && 0 /* Si esta bloqueado por wait */) {
    // proc->parent->status = READY;
    // proc->parent->exit = proc->exit;
  } else {
    /* El proceso es huerfano, directamente se llama a reap */
    proc_reap(proc);
  }
}

/**
 * Recoge un proceso. Es decir, lo elimina de la tabla de
 * procesos y libera sus recursos.
 * Idealmente esta funcion debe ser llamada por aquel que
 * recolecta el resultado del proceso.
 */
int proc_reap(struct proc *proc) {
  process_table[proc->pid] = NULL;
  process_count--;

  kmm_free(proc, kernel_mem);

  return 0;
}

int proc_list(proc_info_t *buffer, int max_count, int *out_count) {
  int count = 0;
  for (int i = 0; i < MAX_PROCESSES && count < max_count; ++i) {
    if (process_table[i] != NULL) {
      buffer[count].pid = process_table[i]->pid;
      buffer[count].ppid =
          process_table[i]->parent ? process_table[i]->parent->pid : -1;
      str_ncpy(buffer[count].name, process_table[i]->name,
               sizeof(buffer[count].name) - 1);
      buffer[count].name[sizeof(buffer[count].name) - 1] = '\0';
      buffer[count].state = process_table[i]->status;
      buffer[count].priority = process_table[i]->priority;
      count++;
    }
  }
  if (out_count) {
    *out_count = count;
  }
  return 0;
}

proc_t *get_proc_by_pid(pid_t pid) {
  return process_table[pid] ? process_table[pid] : NULL;
}
