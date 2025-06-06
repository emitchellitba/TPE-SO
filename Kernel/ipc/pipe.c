#include <pipe.h>

#include <lib.h>
#include <queue.h>

static file_ops_t pipe_read_ops;
static file_ops_t pipe_write_ops;

typedef struct pipe_t {
  struct ringbuf *buffer;
  int readers;
  int writers;
  uint64_t read_sem;
  uint64_t write_sem;
  char id[32];
  bool in_use;
  struct queue *read_queue;
  struct queue *write_queue;
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

int open_pipe(fd_entry_t *fd, const char *id, int mode) {
  if (!fd || !id || (mode != 0 && mode != 1))
    return -1;

  pipe_t *pipe = find_pipe_by_id(id);
  if (!pipe) {
    pipe = create_pipe(id);
    if (!pipe)
      return -1;
  }

  fd->resource = pipe;
  fd->type = FD_PIPE;

  if (mode == 0) {
    pipe->readers++;
    fd->ops = &pipe_read_ops;
  } else {
    pipe->writers++;
    fd->ops = &pipe_write_ops;
  }
  return 0;
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

      pipe->read_queue = queue_new();
      if (!pipe->read_queue) {
        ringbuf_free(pipe->buffer);
        kmm_free(pipe, kernel_mem);
        return NULL;
      }

      pipe->write_queue = queue_new();
      if (!pipe->write_queue) {
        queue_free(pipe->read_queue);
        ringbuf_free(pipe->buffer);
        kmm_free(pipe, kernel_mem);
        return NULL;
      }

      pipes_table.pipes[i] = pipe;

      return pipe;
    }
  }
  return NULL;
}

void pipe_free(struct pipe_t *pipe) {
  if (pipe) {
    pipe->in_use = false;

    if (pipe->buffer)
      ringbuf_free(pipe->buffer);
    kmm_free(pipe, kernel_mem);
  }
}

static ssize_t pipe_read(pipe_t *pipe, void *buf, size_t size) {
  while (!ringbuf_available(pipe->buffer)) {
    enqueue(pipe->read_queue, get_running());
    block_current(0, NULL);
  }

  proc_ready(dequeue(pipe->write_queue));

  return ringbuf_read(pipe->buffer, size, (char *)buf);
}

static ssize_t pipe_write(pipe_t *pipe, const void *buf, size_t size) {
  size_t total_written = 0;
  const char *cbuf = (const char *)buf;

  while (total_written < size) {
    int n =
        ringbuf_write(pipe->buffer, size - total_written, cbuf + total_written);
    total_written += n;

    if (n > 0) {
      proc_ready(dequeue(pipe->read_queue));
    }

    if (total_written < size) {
      enqueue(pipe->write_queue, get_running());
      block_current(0, NULL);
    }
  }

  return total_written;
}

static int pipe_close_read(pipe_t *pipe) {
  if (!pipe)
    return -1;
  if (pipe->readers > 0)
    pipe->readers--;
  if (pipe->readers == 0 && pipe->writers == 0)
    pipe_free(pipe);
  return 0;
}

static int pipe_close_write(pipe_t *pipe) {
  if (!pipe)
    return -1;
  if (pipe->writers > 0)
    pipe->writers--;
  if (pipe->readers == 0 && pipe->writers == 0)
    pipe_free(pipe);
  return 0;
}

static int pipe_add_ref_write(pipe_t *pipe) {
  if (!pipe)
    return -1;
  pipe->writers++;
  return 0;
}

static int pipe_add_ref_read(pipe_t *pipe) {
  if (!pipe)
    return -1;
  pipe->readers++;
  return 0;
}

static file_ops_t pipe_read_ops = {.read = pipe_read,
                                   .write = NULL,
                                   .close = pipe_close_read,
                                   .add_ref = pipe_add_ref_read};

static file_ops_t pipe_write_ops = {.read = NULL,
                                    .write = pipe_write,
                                    .close = pipe_close_write,
                                    .add_ref = pipe_add_ref_write};
