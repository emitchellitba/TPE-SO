// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

/* Modulo que intermedia entre las syscalls y el acceso a la terminal */
#include <tty.h>

#include <keyboardDriver.h>
#include <lib/error.h>
#include <process.h>
#include <stdlib.h>

proc_t *foreground_process = NULL;

/* Esta funcion le indica al tty que debe matar al proceso foreground */
void signal_kill_tty() { proc_kill(foreground_process, 0); }

int read_from_keyboard(char *buffer, size_t count) {
  if (!foreground_process || count == 0 || !buffer)
    return -EINVAL;

  if (get_running() != foreground_process)
    return -EPERM;

  int n = read_line(buffer, count);

  if (buffer[0] == VEOF) { /* Ctrl+D */
    buffer[0] = '\0';
    return 0; /* EOF */
  }

  return n;
}

/* Funcion segura para ceder el foreground del caller a proc */
void set_foreground_process(proc_t *proc, proc_t *caller) {
  if (!proc)
    return;

  if (proc->pid == 1) {
    /* Es el proceso init */
    foreground_process = proc;
    foreground_process->mode = FG;
    return;
  }

  if (!caller)
    return;

  proc_t *prev = caller;

  if (prev->mode == FG) {
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
