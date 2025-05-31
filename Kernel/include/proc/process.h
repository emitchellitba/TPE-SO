#ifndef PROC_H
#define PROC_H

#include "../ds/queue.h"
#include <stdint.h>

/** TODO: Crear una seccion para codigos de errores */
#define NOMEMERR 1;
#define INVALARGSERR 2;

#define STACK_SIZE (8192U) /* 8 KiB */
#define QUANTUM_DEFAULT 2

typedef int (*proc_main_function)(int argc, char **argv);

/*
  IDEA FUNCIONAMIENTO PROCESOS:

  - Cada proceso tiene un PCB (Process Control Block) que contiene toda la
  informacion necesaria para su manejo.

  Cuando un proceso termina:
    1. Se guarda el valor de retorno en el PCB
    2. Se cambia el estado del proceso a ZOMBIE (termino pero no fue
  recolectado)
    3. Se libera la memoria utilizada por el proceso

  Cuando un proceso padre llama a waitpid(pid):
    1. Se busca el PCB del proceso hijo en la cola de procesos
    2. Si el proceso hijo no ha terminado, El proceso padre se bloquea y se
  agrega a la cola de espera del hijo (obs: los hijos tienen un unico padre, por
  lo que, no es necesario hacer una cola, simplemente basta con que cuando el
  hijo termine, el kernel despierte al padre si esta dormido). Cuando un proceso
  hijo termina, el kernel debe despertar al padre y retornar el valor de retorno
  del hijo en la syscall.
    3. Si el proceso hijo ha terminado, el padre encuentra que el estado del
  hijo es ZOMBIE, el kernel agarra el estado retorno del hijo y se lo pasa al
  padre (valor de retorno de la syscall), luego se liberan los recursos del hijo
  y se cambia su estado a DEAD (que en realidad significa que el PCB se remueve
  de manera completa).
*/

typedef uint8_t pid_t;
typedef uint8_t priority_t;

/** Estados de un proceso */
#define DEAD 0 // Este estado puede ser innecesario
#define READY 1
#define RUNNING 2
#define ZOMBIE 3
#define BLOCKED 4

typedef struct proc {
  pid_t pid;
  const char *name;
  struct proc *parent; // Puntero al proceso padre

  uint64_t *stack_start;   // Comienzo del stack: direccion mas alta
  uint64_t *stack_pointer; // Posicion actual del stack

  // char **argv;
  // uint64_t argc;

  proc_main_function entry; // Direccion de inicio del proceso

  struct queue
      wait_queue; // Cola de procesos hijos que hay que esperar con wait

  /** Valor de retorno del proceso. -1 indica que no terminó */
  int exit;

  int status; // Estado del proceso (0 = running, 1 = ready, 2 = zombie)
  priority_t has_quantum;
  priority_t priority;   

  // TODO: Agregar fds (ver como agregarlos)

  // PARA DEBUGGING
  uint64_t start_time;
  uint64_t cpu_time;

  /** por si queremos implementar seniales */
  // struct queue *sig_queue; // Cola de señales
  // struct sigaction sigaction[SIG_MAX + 1]; // Array de handlers
} proc_t;

int proc_new(proc_t **ref);
int proc_init(proc_t *proc, const char *name, proc_t *parent,
              proc_main_function entry);

#endif // PROC_H
