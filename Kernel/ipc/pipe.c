#include <pipe.h>

#include <lib.h>
#include <queue.h>

typedef struct pipe_t {
  struct ringbuf *buffer;
  int readers;
  int writers;
  uint64_t read_sem;
  uint64_t write_sem;
  char id[32];
  bool in_use;
} pipe_t;

typedef struct {
  pipe_t *pipes[MAX_PIPES];
} pipes_table_t;

pipes_table_t pipes_table = {0};

pipe_t *find_pipe_by_id(const char *id) {
  for (int i = 0; i < MAX_PIPES; i++) {
    if (pipes_table.pipes[i] && pipes_table.pipes[i]->in_use &&
        str_cmp(pipes_table.pipes[i]->id, id) == 0) {
      return pipes_table.pipes[i];
    }
  }
  return NULL;
}

pipe_t *create_pipe(const char *id) {
  for (int i = 0; i < MAX_PIPES; i++) {
    if (!pipes_table.pipes[i]) {
      pipe_t *pipe = kmalloc(kernel_mem, sizeof(pipe_t));
      if (!pipe) {
        return NULL;
      }

      pipe->buffer = ringbuf_new(PIPE_BUFFER_SIZE);
      if (!pipe->buffer) {
        kmm_free(pipe, kernel_mem);
        return NULL;
      }

      str_ncpy(pipe->id, id, sizeof(pipe->id));
      pipe->in_use = true;
      pipes_table.pipes[i] = pipe;

      return pipe;
    }
  }
  return NULL;
}

void pipe_free(struct pipe_t *pipe) {
  if (pipe) {
    if (pipe->buffer)
      ringbuf_free(pipe->buffer);
    kmm_free(pipe, kernel_mem);
  }
}

ssize_t pipe_read(pipe_t *pipe, size_t size, void *buf) {
  for (int i = 0; i < size; i++) {
    // wait(pipe->read_sem);

    ringbuf_read(pipe->buffer, 1, (char *)buf + i);

    // signal(pipe->write_sem);
  }

  return size;
}

ssize_t pipe_write(pipe_t *pipe, size_t size, const void *buf) {
  size_t i = 0;

  for (; i < size; i++) {
    // wait(pipe->write_sem);

    ringbuf_write(pipe->buffer, 1, (const char *)buf + i);

    // signal(pipe->read_sem);
  }

  return i;
}
