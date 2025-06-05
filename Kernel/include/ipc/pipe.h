#ifndef _PIPE_H
#define _PIPE_H

#include <memory_manager.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>

KMEMORY_DECLARE

#define PIPE_BUFFER_SIZE 1024
#define MAX_PIPES 64

typedef struct pipe_t pipe_t;
typedef struct pipe_table_t pipe_table_t;

pipe_t *find_pipe_by_id(const char *id);
pipe_t *create_pipe(const char *id);
void pipe_free(struct pipe_t *pipe);
ssize_t pipe_read(pipe_t *pipe, size_t size, void *buf);
ssize_t pipe_write(pipe_t *pipe, size_t size, const void *buf);

#endif // _PIPE_H
