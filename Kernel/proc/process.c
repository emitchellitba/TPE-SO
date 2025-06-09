#include <process.h>

#include "../include/lib/memory_manager.h"
#include <ds/queue.h>
#include <filedesc.h>
#include <lib/logger.h>
#include <stdbool.h>
#include <tty.h>

static int proc_log_level = LOG_DEBUG;
LOGGER_DEFINE(proc, proc_log, proc_log_level)

#define MAX_PROCESSES 64
static struct proc *process_table[MAX_PROCESSES] = {NULL};
static int process_count = 0;

/* ---- FUNCIONES AUXILIARES ESTATICAS ----- */

static pid_t proc_pid_alloc() {
  for (pid_t i = 0; i < MAX_PROCESSES; i++) {
    if (process_table[i] == NULL) {
      return i; // Retorna el primer PID disponible
    }
  }
  proc_log(LOG_ERR, "No available PIDs\n");
  return -1; // No hay PIDs disponibles
}

static void reparent_to_init(proc_t *child);

/* ---- FUNCIONES DEL MODULO ----- */

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
 * Inicializa el stack, fds y la informacion del proceso.
 * Recibe un arreglo de file descriptors que heredara del padre.
 * Si no se pasa arreglo, se inicializan los fds por defecto
 */
int proc_init(proc_t *proc, const char *name, proc_main_function entry,
              int fds[], int background) {
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

  /* Inicializando file descriptors */
  if (fds)
    inherit_fds(proc, get_running(), fds);
  else
    set_default_fds(proc);

  if (!background) { /* Ceder foreground */
    set_foreground_process(proc);
  }

  proc_t *parent = get_running();
  proc->name = name;
  proc->parent = parent;
  proc->entry = entry;

  proc->priority = QUANTUM_DEFAULT;

  proc->status = READY;
  proc->exit = -1;

  proc->block_reason = BLK_NONE;
  proc->waiting_on = NULL;

  if (parent && parent->child_count < MAX_PROCESSES) {
    parent->children[parent->child_count++] = proc->pid;
  } else if (parent) {
    proc_log(LOG_ERR, "Parent process has reached maximum children limit\n");
    return -1; // No se puede agregar más hijos
  }

  return 0;
}

/**
 * Mata un proceso. Cambia su estado a ZOMBIE y libera sus recursos del kernel.
 */
void proc_kill(struct proc *proc, int exit_code) {
  if (!proc) {
    proc_log(LOG_ERR, "Cannot kill a NULL process\n");
    return;
  }

  if (proc->pid == 0 || proc->pid == 1) {
    proc_log(LOG_ERR, "Cannot kill the init process (PID: %d)\n", proc->pid);
    return;
  }

  proc_log(LOG_INFO, "Killing process %s (PID: %d)\n", proc->name, proc->pid);

  /* Devolver el foreground */
  set_foreground_process(proc->parent);

  /* Eliminarlo del flujo del scheduler */
  if (proc->status == RUNNING) {
    sched_rm_current();
  }
  queue_remove(proc->waiting_on, proc);

  /* Cambiar estado del proceso */
  proc->status = ZOMBIE;
  proc->exit = exit_code;

  /* Liberar recursos del kernel utilizados por el proceso (stack) */
  kmm_free(proc->stack_start, kernel_mem);

  /* Cerrar todos los file descritptors */
  for (int i = 0; i < FD_MAX; i++) {
    if (proc->fds[i].ops && proc->fds[i].ops->close)
      proc->fds[i].ops->close(proc->fds[i].resource);

    proc->fds[i] = (fd_entry_t){.resource = NULL, .ops = NULL, .type = FD_NONE};
  }

  /* Reparentar a todos los hijos */
  for (int i = 0; i < proc->child_count; i++) {
    proc_t *child = get_proc_by_pid(proc->children[i]);
    if (child)
      reparent_to_init(child);
  }

  /* Despertar al padre si lo hay y está esperando al hijo */
  if (proc->parent && proc->parent->status == BLOCKED &&
      proc->parent->block_reason == BLK_CHILD) {
    proc_ready(proc->parent);
  } else if (!proc->parent) {
    /* Caso raro: no tiene padre, simplemente reap */
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
  /* Liberar el nombre */
  kmm_free((void *)proc->name, kernel_mem);
  proc->name = NULL;

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
      if (process_table[i]->name) {
        str_ncpy(buffer[count].name, process_table[i]->name,
                 sizeof(buffer[count].name) - 1);
      } else {
        str_ncpy(buffer[count].name, "<CLEANED>",
                 sizeof(buffer[count].name) - 1);
      }
      buffer[count].name[sizeof(buffer[count].name) - 1] = '\0';
      buffer[count].state = process_table[i]->status;
      buffer[count].priority = process_table[i]->priority;
      buffer[count].mode = process_table[i]->mode;
      buffer[count].stack_base_address =
          (uint64_t)process_table[i]->stack_start;
      buffer[count].current_stack_pointer =
          (uint64_t)process_table[i]->stack_pointer;
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

pid_t wait_pid(pid_t pid, int *exit_status) {
  proc_t *parent = get_running();
  proc_t *child = NULL;

  if (parent->child_count == 0) {
    proc_log(LOG_INFO, "No child processes to wait for\n");
    return -1;
  }

  while (true) {
    if (pid == -1) {
      for (int i = 0; i < parent->child_count; i++) {
        child = get_proc_by_pid(parent->children[i]);
        if (child && child->status == ZOMBIE) {
          if (exit_status)
            *exit_status = child->exit;
          int child_pid = child->pid;

          proc_reap(child);

          for (int j = i; j < parent->child_count - 1; j++)
            parent->children[j] = parent->children[j + 1];
          parent->child_count--;

          return child_pid;
        }
      }

      block_current(BLK_CHILD, NULL);
    } else {
      int found = 0;
      for (int i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == pid) {
          found = 1;
          child = get_proc_by_pid(pid);
          if (!child)
            return -1;

          if (child->status == ZOMBIE) {
            if (exit_status)
              *exit_status = child->exit;
            proc_reap(child);
            for (int j = i; j < parent->child_count - 1; j++)
              parent->children[j] = parent->children[j + 1];
            parent->child_count--;
            return pid;
          }

          block_current(BLK_CHILD, NULL);
          break;
        }
      }

      if (!found) {
        return -1;
      }
    }
  }
}

/* No utilizada pero es de utilidad para syscalls bloqueantes */
void return_from_syscall(proc_t *proc, int retval) {
  if (!proc) {
    proc_log(LOG_ERR, "Cannot return from syscall for a NULL process\n");
    return;
  }

  proc->syscall_retval = retval;
}

int find_free_fd(proc_t *proc) {
  if (!proc)
    return -1;

  for (int i = 0; i < FD_MAX; i++) {
    if (proc->fds[i].type == FD_NONE)
      return i;
  }
  return -1;
}

int copy_fd(proc_t *target, proc_t *source, int targetfd, int srcfd) {
  if (source->fds[srcfd].type == FD_NONE) {
    proc_log(LOG_ERR, "Source file descriptor is not valid\n");
    return -1;
  }

  target->fds[targetfd] = source->fds[srcfd];

  return 0;
}

int64_t change_priority(pid_t pid, priority_t new_priority) {
  proc_t *parent = get_running();
  proc_t *child = NULL;
  int found = 0;

  for (int i = 0; i < parent->child_count && found == 0; i++) {
    if (parent->children[i] == pid) {
      found = 1;
    }
  }

  if (!found) {
    proc_log(LOG_ERR, "PID %d is not a child of current process (PID %d)\n",
             pid, parent->pid);
    return -1;
  }

  child = get_proc_by_pid(pid);
  if (!child) {
    proc_log(LOG_ERR, "Process with PID %d not found\n", pid);
    return -1;
  }

  if (new_priority <= 0 || new_priority > QUANTUM_MAX) {
    proc_log(LOG_ERR, "Invalid priority value: %d\n", new_priority);
    return -1;
  }

  child->priority = new_priority;
  child->has_quantum = new_priority;
  proc_log(LOG_INFO, "Changed priority of PID %d to %d\n", pid, new_priority);
  return 0;
}

static void reparent_to_init(proc_t *child) {
  if (!child)
    return -1;

  proc_t *init_process = get_proc_by_pid(1);

  child->parent = init_process;

  if (init_process->child_count < MAX_PROCESSES) {
    init_process->children[init_process->child_count++] = child->pid;
  } else {
    child->parent = NULL;
    return -1;
  }

  return 0;
}
