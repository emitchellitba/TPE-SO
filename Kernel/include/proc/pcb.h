#ifndef _PCB_H_
#define _PCB_H_

#include <proc_info.h>
#include <stdint.h>
#include <sys/types.h>

#define BG 0
#define FG 1

typedef enum { FD_NONE, FD_PIPE, FD_TERMINAL } FDType;

typedef struct file_ops {
  ssize_t (*read)(void *resource, void *buf, size_t count);
  ssize_t (*write)(void *resource, const void *buf, size_t count);
  int (*close)(void *resource);
  int (*add_ref)(void *resource);
} file_ops_t;

typedef struct fd_entry {
  void *resource;
  file_ops_t *ops;
  FDType type;
} fd_entry_t;

typedef uint8_t priority_t;

typedef int (*proc_main_function)(int argc, char **argv);

#define FD_MAX 16
#define MAX_CHILDREN 64

typedef struct proc {
  pid_t pid;
  const char *name;

  struct proc *parent;          // Puntero al proceso padre
  pid_t children[MAX_CHILDREN]; // Arreglo de PIDs de hijos
  int child_count;              // Cantidad de hijos

  uint64_t *stack_start;   // Comienzo del stack: direccion mas alta
  uint64_t *stack_pointer; // Posicion actual del stack

  proc_main_function entry; // Direccion de inicio del proceso

  block_reason_t block_reason;
  struct queue *waiting_on;

  /** Valor de retorno del proceso. -1 indica que no terminó */
  int exit;

  /** Valor de retorno de la ultima syscall */
  int syscall_retval; // No utilizado pero es de utilidad para futuras syscalls
                      // bloqueantes

  proc_state_t
      status; // Estado del proceso (0 = running, 1 = ready, 2 = zombie)
  priority_t has_quantum;
  priority_t priority;

  int mode; // FG o BG

  fd_entry_t fds[FD_MAX]; // Tabla de descriptores de archivos
} proc_t;

#endif // _PCB_H_
