#ifndef _PROC_INFO_H
#define _PROC_INFO_H

#include <stdint.h>

typedef enum block_reason
{
  BLK_NONE,      // No está bloqueado o razón no específica
  BLK_KEYBOARD,  // Esperando entrada del teclado
  BLK_ARBITRARY, // Bloqueado por syscall sys_block
  BLK_SEMAPHORE, // Esperando en un semáforo
  BLK_CHILD,      // Esperando a un hijo
  BLK_SLEEP,      // Esperando a que un proceso hijo termine (waitpid)
  BLK_PIPE_READ,  // Esperando para leer de un pipe
  BLK_PIPE_WRITE, // Esperando para escribir en un pipe
  BLK_TERMINAL,   // Esperando en un terminal
} block_reason_t;

typedef enum proc_state_t
{
  DEAD,
  READY,
  RUNNING,
  ZOMBIE,
  BLOCKED
} proc_state_t;

typedef struct
{
  int pid;
  int ppid;
  char name[32];
  int state;
  int priority;
  int mode;
  int block_reason;
  uint64_t stack_base_address;
  uint64_t current_stack_pointer;
} proc_info_t;

#define QUANTUM_MAX 5

#endif // _PROC_INFO_H
