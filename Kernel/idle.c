#include <kernel.h>
#include <kernel_asm.h>

int idle_main(void) {
  while (1)
    _hlt();
}

int initialize_idle(void) {
  char *name = "idle";
  fs_load(name, &idle_main);

  char *argv[] = {name, NULL};
  int argc = 1;

  return syscall_dispatcher(SPAWN_PROCESS_SYSCALL_ENTRY, (uint64_t)name, argc,
                            argv);
}
