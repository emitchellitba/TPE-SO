#include <ipc/filedesc.h>

#include <process.h>

extern file_ops_t keyboard_ops;
extern file_ops_t video_ops;
extern file_ops_t video_err_ops;

void set_default_fds(proc_t *proc) {
  if (!proc) {
    return;
  }

  proc->fds[0] = (fd_entry_t){
      .resource = NULL,
      .ops = &keyboard_ops,
      .type = FD_TERMINAL,
  };
  proc->fds[1] = (fd_entry_t){
      .resource = NULL,
      .ops = &video_ops,
      .type = FD_TERMINAL,
  };
  proc->fds[2] = (fd_entry_t){
      .resource = NULL,
      .ops = &video_err_ops,
      .type = FD_TERMINAL,
  };
}

int inherit_fds(proc_t *target, proc_t *source, int fds[]) {
  if (!target || !source || !fds) {
    return -1;
  }

  for (int i = 0; i < FD_MAX; i++) {
    if (fds[i] < 0 || fds[i] >= FD_MAX)
      break;

    fd_entry_t *source_fd = &source->fds[fds[i]];
    if (source_fd->ops && source_fd->ops->add_ref) {
      source_fd->ops->add_ref(source_fd->resource);
    }

    target->fds[i] = *source_fd;
  }

  return 0;
}
