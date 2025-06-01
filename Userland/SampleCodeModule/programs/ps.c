#include <ps.h>

int ps_main(int argc, char *argv[]) {
  proc_info_t procs[32];
  int n = get_procs(procs, 32);

  for (int i = 0; i < n; i++) {
    printf("PID: %d  PPID: %d  NAME: %s  STATE: %d\n", procs[i].pid,
           procs[i].ppid, procs[i].name, procs[i].state);
  }

  return 0;
}
