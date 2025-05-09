#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <stdint.h>

void exceptionDispatcher(int exception);
void printRegisters();
static void zero_division();
static void invalid_operation_code();

#endif