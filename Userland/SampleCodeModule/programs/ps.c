#include <ps.h>

#include <libu.h>
#include <stdLibrary.h>

static const char *state_names[] = {"DEAD", "READY", "RUNNING", "ZOMBIE",
                                    "BLOCKED"};

int ps_main(int argc, char *argv[]) {
  proc_info_t procs[32];
  int n = get_procs(procs, 32);

  // Print header
  printf("%-5s %-5s %-16s %-10s %-8s\n", "PID", "PPID", "NAME", "STATE",
         "PRIORITY");

  // Print each process in a table row
  for (int i = 0; i < n; i++) {
    printf("%-5d %-5d %-16s %-10s %-8d\n", procs[i].pid, procs[i].ppid,
           procs[i].name, state_names[procs[i].state], procs[i].priority);
  }

  proc_exit(0);

  return 0;
}
