#include <pipe.h>

#include <kernel_asm.h>
#include <lib.h>
#include <lib/error.h>
#include <queue.h>
#include <semaphore.h>

static file_ops_t pipe_read_ops;
static file_ops_t pipe_write_ops;

typedef struct pipe_t {
  struct ringbuf *buffer;
  int readers;
  int writers;
  char id[32];
  bool in_use;
  uint8_t lock;
  struct queue *readers_wait_queue;
  struct queue *writers_wait_queue;
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

      pipe->lock = 1;
      pipe->readers = 0;
      pipe->writers = 0;
      pipe->readers_wait_queue = queue_new();
      if (!pipe->readers_wait_queue) {
        ringbuf_free(pipe->buffer);
        kmm_free(pipe, kernel_mem);
        return NULL;
      }
      pipe->writers_wait_queue = queue_new();
      if (!pipe->writers_wait_queue) {
        queue_free(pipe->readers_wait_queue);
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
  if (!pipe)
    return;

  pipe->in_use = false;

  ringbuf_free(pipe->buffer);
  queue_free(pipe->readers_wait_queue);
  queue_free(pipe->writers_wait_queue);

  kmm_free(pipe, kernel_mem);
}

static ssize_t pipe_read(void *resource, void *buf, size_t size) {
  pipe_t *pipe = (pipe_t *)resource;
  acquire(&(pipe->lock));

  while (!ringbuf_available(pipe->buffer)) {
    if (pipe->writers == 0) {
      release(&(pipe->lock));
      return 0; // EOF: no writers and no data
    }

    release(&(pipe->lock));
    enqueue(pipe->readers_wait_queue, get_running());
    block_current(BLK_PIPE_READ, pipe->readers_wait_queue);
    acquire(&(pipe->lock));
  }

  size_t n = ringbuf_read(pipe->buffer, size, (char *)buf);

  /* Notificar a escritores */
  proc_ready(dequeue(pipe->writers_wait_queue));

  release(&(pipe->lock));
  return n;
}

static ssize_t pipe_write(void *resource, const void *buf, size_t size) {
  pipe_t *pipe = (pipe_t *)resource;
  acquire(&(pipe->lock));

  while (ringbuf_available(pipe->buffer) >= PIPE_BUFFER_SIZE) {
    if (pipe->readers == 0) {
      release(&(pipe->lock));
      return -EPIPE;
    }

    release(&(pipe->lock));
    enqueue(pipe->writers_wait_queue, get_running());
    block_current(BLK_PIPE_WRITE, pipe->writers_wait_queue);
    acquire(&(pipe->lock));
  }

  size_t n = ringbuf_write(pipe->buffer, size, (char *)buf);

  /* Notificar a lectores */
  proc_ready(dequeue(pipe->readers_wait_queue));

  release(&(pipe->lock));
  return n;
}

static int pipe_close_read(void *resource) {
  pipe_t *pipe = (pipe_t *)resource;
  if (!pipe)
    return -1;
  if (pipe->readers > 0)
    pipe->readers--;
  if (pipe->readers == 0 && pipe->writers == 0)
    pipe_free(pipe);
  return 0;
}

static int pipe_close_write(void *resource) {
  pipe_t *pipe = (pipe_t *)resource;
  if (!pipe)
    return -1;
  if (pipe->writers > 0)
    pipe->writers--;

  if (pipe->writers == 0) {
    while (pipe->readers_wait_queue->count > 0)
      proc_ready(dequeue(pipe->readers_wait_queue));
  }

  if (pipe->readers == 0 && pipe->writers == 0)
    pipe_free(pipe);
  return 0;
}

static int pipe_add_ref_write(void *resource) {
  pipe_t *pipe = (pipe_t *)resource;
  if (!pipe)
    return -1;
  pipe->writers++;
  return 0;
}

static int pipe_add_ref_read(void *resource) {
  pipe_t *pipe = (pipe_t *)resource;
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
