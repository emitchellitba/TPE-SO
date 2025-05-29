#ifndef EXEC_H
#define EXEC_H

#include <lib.h>
#include <logger.h>
#include <memory_manager.h>
#include <process.h>
#include <scheduler.h>

#define X86_RFLAGS 0x202
#define X86_CS 0x8
#define X86_SS 0x0

int execv(struct proc *proc, int argc, char *const argv[]);

#endif // EXEC_H
