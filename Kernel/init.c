#include <errno.h>
#include <lib/logger.h>
#include <stdlib.h>

#define SHELL_ENTRY_POINT_ADDRESS ((int (*)(int, char **))0x400000)

extern void call_timer_tick();
extern int64_t kernel_syscall_create_process(const char *name,
                                             int (*entry_point)(int, char **),
                                             int argc, char *argv[]);

int init_main(int argc, char **argv) {
  // int64_t current_pid = syscall_get_pid();
  // printk("Init process started (PID: %ld)\n", current_pid);
  printk("Init process started\n");

  char *shell_argv_data[] = {"shell", NULL}; // argv for shell: {"shell", NULL}
  int shell_argc = 1;                        // argc for shell: 1

  int (*shell_entry_point)(int, char **) = SHELL_ENTRY_POINT_ADDRESS;

  int64_t shell_pid = kernel_syscall_create_process(
      "shell", shell_entry_point, shell_argc, shell_argv_data);

  if (shell_pid >= 0) {
    printk("Init: Shell process created successfully. ");
  } else {
    printk("Init: Failed to create shell process. ");
  }

  // init no termina
  while (1)
    ;
}
