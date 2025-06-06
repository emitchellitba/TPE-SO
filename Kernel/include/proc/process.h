#ifndef PROC_H
#define PROC_H

#include <ds/queue.h>
#include <ds/ringbuf.h>
#include <filedesc.h>
#include <lib.h>
#include <stdint.h>

/* TODO: Crear una seccion para codigos de errores */
#define NOMEMERR 1;
#define INVALARGSERR 2;

#define STACK_SIZE (8192U) /* 8 KiB */
#define QUANTUM_DEFAULT 2

#define FD_MAX 16

typedef int (*proc_main_function)(int argc, char **argv);

typedef struct {
  int pid;
  int ppid;
  char name[32];
  int state;
  int priority;
} proc_info_t;
// TODO: MOVER ESTE proc_info_t a una librera compartida entre kernel y userland

typedef uint8_t priority_t;

// En tu process.h o similar
typedef enum proc_state_t {
  DEAD,
  READY,
  RUNNING,
  ZOMBIE,
  BLOCKED

} proc_state_t;

typedef enum block_reason {
  BLK_NONE,      // No está bloqueado o razón no específica
  BLK_KEYBOARD,  // Esperando entrada del teclado
  BLK_ARBITRARY, // Bloqueado por syscall sys_block
  BLK_SEMAPHORE, // Esperando en un semáforo
  BLK_CHILD      // Esperando a que un proceso hijo termine (waitpid)
                 // BLK_PIPE_READ,  // (Futuro) Esperando para leer de un pipe
  // BLK_PIPE_WRITE, // (Futuro) Esperando para escribir en un pipe
} block_reason_t;

typedef struct proc {
  pid_t pid;
  const char *name;
  struct proc *parent; // Puntero al proceso padre

  uint64_t *stack_start;   // Comienzo del stack: direccion mas alta
  uint64_t *stack_pointer; // Posicion actual del stack

  proc_main_function entry; // Direccion de inicio del proceso

  block_reason_t block_reason;
  void *waiting_on;

  // queue_t my_queue; // Cola de espera para bloqueos

  /** Valor de retorno del proceso. -1 indica que no terminó */
  int exit;

  /** Valor de retorno de la ultima syscall */
  int syscall_retval;

  proc_state_t
      status; // Estado del proceso (0 = running, 1 = ready, 2 = zombie)
  priority_t has_quantum;
  priority_t priority;

  fd_entry_t fds[FD_MAX]; // Tabla de descriptores de archivos

  // PARA DEBUGGING
  uint64_t start_time;
  uint64_t cpu_time;

  /** por si queremos implementar seniales */
  // struct queue *sig_queue; // Cola de señales
  // struct sigaction sigaction[SIG_MAX + 1]; // Array de handlers
} proc_t;

extern file_ops_t video_ops;
extern file_ops_t video_err_ops;
extern file_ops_t keyboard_ops;

extern void sched_current_died();
extern void sched_ready_queue_remove(struct proc *proc);

int proc_new(proc_t **ref);
int proc_init(proc_t *proc, const char *name, proc_t *parent,
              proc_main_function entry, int redirect, int red_fds[2]);
int proc_list(proc_info_t *buffer, int max_count, int *out_count);
void proc_kill(struct proc *proc);
int proc_reap(struct proc *proc);

proc_t *get_proc_by_pid(pid_t pid);
void return_from_syscall(proc_t *proc, int retval);
int find_free_fd(proc_t *proc);
int copy_fd(proc_t *target, proc_t *source, int target_fd, int src_fd);

#endif // PROC_H
