/* Modulo que intermedia entre las syscalls y el acceso a la terminal */
#include <tty.h>

#include <stdlib.h>

proc_t *foreground_process = NULL;

int read_from_keyboard(char *buffer, size_t count) {
  if (!foreground_process || count == 0 || !buffer)
    return -1;

  if (get_running() != foreground_process) {
    return -2;
  }

  return read_line(buffer, count);
}

/* Funcion segura para ceder el foreground */
void set_foreground_process(proc_t *proc) {
  if (!proc)
    return;

  if (proc->pid == 1) {
    /* Es el proceso init */
    foreground_process = proc;
    foreground_process->mode = FG;
    return;
  }

  proc_t *prev = get_running();

  if (prev && prev->mode == FG) {
    prev->mode = BG;
  } else { /* El proceso corriendo no tiene el foreground para cederlo */
    return;
  }

  foreground_process = proc;
  foreground_process->mode = FG;
}

void reset_foreground_process() { foreground_process = NULL; }

/* Se exportan los file descriptors de terminal */

static ssize_t video_write_stdout(void *resource, const void *buf,
                                  size_t count) {
  const char *cbuf = (const char *)buf;

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

  int n = read_from_keyboard(cbuf, count);

  return n;
}

file_ops_t keyboard_ops = {
    .read = kb_read, .write = NULL, .close = NULL, .add_ref = NULL};

file_ops_t video_ops = {
    .read = NULL, .write = video_write_stdout, .close = NULL, .add_ref = NULL};

file_ops_t video_err_ops = {
    .read = NULL, .write = video_write_stderr, .close = NULL, .add_ref = NULL};
