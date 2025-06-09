#ifndef PROC_INFO_H
#define PROC_INFO_H

typedef enum proc_state_t {
  DEAD,
  READY,
  RUNNING,
  ZOMBIE,
  BLOCKED
} proc_state_t;

typedef struct {
  int pid;
  int ppid;
  char name[32];
  int state;
  int priority;
} proc_info_t;

#define QUANTUM_MAX 5

#endif