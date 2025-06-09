#include <libu.h>
#include <stdLibrary.h>

static const char *state_names[] = {"DEAD", "READY", "RUNNING", "ZOMBIE",
                                    "BLOCKED"};

static const char *block_reason_names[] = {
    "None",  "Keyboard",  "Arbitrary",  "Semaphore", "Child",
    "Sleep", "Pipe Read", "Pipe Write", "Terminal"};

int ps_main(int argc, char *argv[]) {
  proc_info_t procs[32];
  int n = get_procs(procs, 32);

  printf("%-5s %-5s %-16s %-10s %-16s %-12s %-9s %-18s %-18s\n", "pid", "ppid",
         "Name", "State", "Block reason", "Priority", "Run mode", "Stack base",
         "Stack ptr");

  for (int i = 0; i < n; i++) {
    const char *block_reason = (procs[i].state == BLOCKED)
                                   ? block_reason_names[procs[i].block_reason]
                                   : "-";
    const char *ppid_str = (procs[i].ppid < 0) ? "-" : NULL;
    if (ppid_str)
      printf("%-5d %-5s %-16s %-10s %-16s %-12d %-9s 0x%-16x 0x%-16x\n",
             procs[i].pid, ppid_str, procs[i].name, state_names[procs[i].state],
             block_reason, procs[i].priority, procs[i].mode ? "FG" : "BG",
             procs[i].stack_base_address, procs[i].current_stack_pointer);
    else
      printf("%-5d %-5d %-16s %-10s %-16s %-12d %-9s 0x%-16x 0x%-16x\n",
             procs[i].pid, procs[i].ppid, procs[i].name,
             state_names[procs[i].state], block_reason, procs[i].priority,
             procs[i].mode ? "FG" : "BG", procs[i].stack_base_address,
             procs[i].current_stack_pointer);
  }

  return 0;
}
