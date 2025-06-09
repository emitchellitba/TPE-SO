#ifndef PROC_H
#define PROC_H

#include <pcb.h>
#include <proc_info.h>
#include <queue.h>
#include <stdint.h>
#include <sys/types.h>

/* TODO: Crear una seccion para codigos de errores */
#define NOMEMERR 1;
#define INVALARGSERR 2;

#define STACK_SIZE (8192U) /* 8 KiB */
#define QUANTUM_DEFAULT 2

extern file_ops_t video_ops;
extern file_ops_t video_err_ops;
extern file_ops_t keyboard_ops;

int proc_new(proc_t **ref);

int proc_init(proc_t *proc, const char *name, proc_main_function entry,
              int fds[], int background);

int proc_list(proc_info_t *buffer, int max_count, int *out_count);

void proc_kill(struct proc *proc, int exit_code);

int proc_reap(struct proc *proc);

pid_t wait_pid(pid_t pid, int *exit_status);

proc_t *get_proc_by_pid(pid_t pid);

int64_t change_priority(pid_t pid, priority_t new_priority);

void return_from_syscall(proc_t *proc, int retval);

int find_free_fd(proc_t *proc);

int copy_fd(proc_t *target, proc_t *source, int target_fd, int src_fd);

#endif // PROC_H
