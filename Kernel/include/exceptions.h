#ifndef _exceptions_
#define _exceptions_

#include <stdint.h>


void exceptionDispatcher(int exception);
void printRegisters();
static void zero_division();
static void invalid_operation_code();


#endif 