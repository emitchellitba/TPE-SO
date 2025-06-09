#include <libu.h>
#include <stdLibrary.h>
#include <sys/types.h>

int block_main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <pid>\n", argv[0]);
    return -1;
  }

  pid_t pid = atoi(argv[1]);
  if (pid <= 0) {
    printf("Invalid PID: %d\n", pid);
    return -1;
  }

  proc_info_t procs[32];
  int n = get_procs(procs, 32);

  proc_info_t *target_proc = NULL;
  for (int i = 0; i < n; i++) {
    if (procs[i].pid == pid) {
      target_proc = &procs[i];
      break;
    }
  }

  if (!target_proc) {
    printf("Process with PID %d not found.\n", pid);
    return -1;
  }

  if (target_proc->state == READY || target_proc->state == RUNNING) {
    block_proc(pid);
    printf("Process %d (%s) is now blocked.\n", pid, target_proc->name);
    return 0;
  } else if (target_proc->state == BLOCKED) {
    unblock_proc(pid);
    printf("Process %d (%s) is now ready.\n", pid, target_proc->name);
    return 0;
  } else {
    printf("Process %d (%s) current state cannot be modified.\n", pid,
           target_proc->name);
    return -1;
  }
}
