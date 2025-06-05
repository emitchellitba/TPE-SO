#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <lib.h>
#include <stdint.h>
#include <syscall_dispatcher.h>
#include <videoDriver.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPERATION_CODE_ID 6
#define CANT_REGS 17

void exceptionDispatcher(int exception);
void printRegisters();
static void zero_division();
static void invalid_operation_code();

#endif
