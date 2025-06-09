#ifndef _PROC_INFO_H
#define _PROC_INFO_H

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
  uint64_t stack_base_address;
  uint64_t current_stack_pointer;
} proc_info_t;

#define QUANTUM_MAX 5

#endif // _PROC_INFO_H
