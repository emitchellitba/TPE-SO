#include <libu.h>
#include <stdLibrary.h>

static const char *state_names[] = {"DEAD", "READY", "RUNNING", "ZOMBIE",
                                    "BLOCKED"};

int ps_main(int argc, char *argv[]) {
  proc_info_t procs[32];
  int n = get_procs(procs, 32);

  printf("%-5s %-5s %-16s %-10s %-8s %-3s %-18s %-18s\n", "PID", "PPID", "NAME",
         "STATE", "PRIORITY", "BG", "STACK_BASE", "STACK_PTR");

  for (int i = 0; i < n; i++) {
    printf("%-5d %-5d %-16s %-10s %-8d %-3s %-18lx %-18lx\n", procs[i].pid,
           procs[i].ppid, procs[i].name, state_names[procs[i].state],
           procs[i].priority, procs[i].mode ? "BG" : "FG",
           procs[i].stack_base_address, procs[i].current_stack_pointer);
  }

  return 0;
}
