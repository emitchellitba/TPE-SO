#ifndef _FD_H_
#define _FD_H_

#include <keyboardDriver.h>
#include <stdlib.h>
#include <sys/types.h>
#include <videodriver.h>

typedef enum { FD_NONE, FD_PIPE, FD_TERMINAL } FDType;

// Abstraccion de operaciones de archivo para acceder a
// distintos tipos de recursos.
typedef struct file_ops {
  ssize_t (*read)(void *resource, void *buf, size_t count);
  ssize_t (*write)(void *resource, const void *buf, size_t count);
} file_ops_t;

typedef struct fd_entry {
  void *resource;
  file_ops_t *ops;
  FDType type;
} fd_entry_t;

#endif // _FD_H_
