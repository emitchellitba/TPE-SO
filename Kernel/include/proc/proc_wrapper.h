#ifndef _PROC_WRAPPER_H
#define _PROC_WRAPPER_H

#include <stdint.h>

int process_wrapper(uint64_t user_argc, char **user_argv);

#endif // _PROC_WRAPPER_H
