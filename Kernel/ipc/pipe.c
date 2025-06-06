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
  // wait

  return ringbuf_read(pipe->buffer, size, (char *)buf);

  // post
}

static ssize_t pipe_write(pipe_t *pipe, const void *buf, size_t size) {
  // wait(pipe->write_sem);

  return ringbuf_write(pipe->buffer, size, (const char *)buf);

  // signal(pipe->read_sem);
}

static file_ops_t pipe_read_ops = {.read = pipe_read, .write = NULL};

static file_ops_t pipe_write_ops = {.read = NULL, .write = pipe_write};
