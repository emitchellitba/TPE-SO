#ifndef _CORE_H_
#define _CORE_H_

#include <stdLibrary.h>

#define MAX_PARAMS 10
#define MAX_PARAM_LEN 32

int exit_cmd();
int rm_cmd(int params_count, char params[MAX_PARAMS][MAX_PARAM_LEN]);

#endif // _CORE_H_
