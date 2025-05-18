#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <stddef.h>
#include <stdint.h>

#define DEFINE_WRAPPER(name, args, call_args)                                  \
  int64_t name args { return _##name call_args; }

#endif // WRAPPERS_H
