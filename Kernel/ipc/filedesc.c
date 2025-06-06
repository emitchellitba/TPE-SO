#include <ipc/filedesc.h>

// TODO: Estas operaciones deberian estar en sus respectivos drivers
static ssize_t video_write_stdout(void *resource, const void *buf,
                                  size_t count) {
  const char *cbuf = (const char *)buf;

  // TODO: Mejorar codigo de videoDriver.c
  printStd(cbuf, count);

  return count;
}

static ssize_t video_write_stderr(void *resource, const void *buf,
                                  size_t count) {
  const char *cbuf = (const char *)buf;

  printStd(cbuf, count);

  return count;
}

static ssize_t kb_read(void *resource, void *buf, size_t count) {
  char *cbuf = (char *)buf;

  int n = read_line(cbuf, count);

  return n;
}

file_ops_t keyboard_ops = {.read = kb_read, .write = NULL};

file_ops_t video_ops = {.read = NULL, .write = video_write_stdout};

file_ops_t video_err_ops = {.read = NULL, .write = video_write_stderr};
