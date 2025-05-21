#ifndef PROC_H
#define PROC_H

#include "../ds/queue.h"
#include <stdint.h>

typedef int pid_t;
typedef int priority_t;

typedef struct proc {
  pid_t pid;
  char *name;
  struct proc *parent; // Puntero al proceso padre

  uint64_t heap_start; // Direccion de inicio del heap
  uint64_t heap;       // Puntero justo despues del ultimo byte del heap
                 // Este valor sera incrementado cuando el proceso pida memoria

  uint64_t stack_start;   // Comienzo del stack: direccion mas alta
  uint64_t stack_pointer; // Posicion actual del stack

  uint64_t entry; // Direccion de inicio del proceso

  struct queue
      wait_queue; // Cola de procesos hijos que hay que esperar con waitpid

  int64_t exit;    // Valor de retorno del proceso
  uint64_t status; // Estado del proceso (0 = running, 1 = ready, 2 = zombie, 3
                   // = dead)
  uint64_t priority;

  // TODO: Agregar fds (ver como agregarlos)

  // PARA DEBUGGING
  uint64_t start_time;
  uint64_t cpu_time;
  uint64_t blocked_by_pid; // PID del proceso que lo bloqueo

  /** por si queremos implementar seniales */
  // struct queue *sig_queue; // Cola de señales
  // struct sigaction sigaction[SIG_MAX + 1]; // Array de handlers
} proc_t;

#endif // PROC_H
