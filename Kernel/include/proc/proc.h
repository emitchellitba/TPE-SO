#ifndef PROC_H
#define PROC_H

#include "../ds/queue.h"
#include <stdint.h>

typedef int pid_t;

typedef struct proc {
  /** Process ID */
  pid_t pid;

  /** Process name - XXX */
  char *name;

  /** Parent process */
  struct proc *parent;

  /** Process initial heap pointer */
  uint64_t heap_start;

  /** Process current heap pointer */
  uint64_t heap;

  /** Process entry point */
  uint64_t entry;

  /** Virtual memory regions */
  // struct vm_space vm_space;

  // struct vm_entry *heap_vm;
  // struct vm_entry *stack_vm;

  /** Recieved signals queue */
  // struct queue *sig_queue;

  /** Queue of children to wait */
  struct queue wait_queue;

  /** Registered signal handlers */
  // struct sigaction sigaction[SIG_MAX + 1];

  /** Exit status of process */
  int exit;

  /** Process is running? */
  int running;
} proc_t;

#endif // PROC_H

/*
typedef struct PCB {
  pid_t pid;
  uint64_t rsp;
  uint64_t lowest_stack_address;
  uint8_t status;
  char ** argv;
  uint64_t argc;
  int64_t ret;
  priority_t priority;
  uint8_t killable;
  struct PCB * waiting_me;
  struct PCB * waiting_for;
  int64_t blocked_by_sem;
  fd_t fds[CANT_FDS];
  uint64_t time;
  uint64_t start;
} PCB; */
