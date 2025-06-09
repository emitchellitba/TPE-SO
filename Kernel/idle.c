#include <fs.h>
#include <kernel.h>
#include <kernel_asm.h>

extern int64_t syscall_dispatcher(uint64_t rax, ...);

int idle_main(void) {
  while (1)
    _hlt();
}

int initialize_idle(void) {
  char *name = "idle";
  fs_load(name, (fs_entry_point_t)&idle_main);

  char *argv[] = {name, NULL};
  int argc = 1;

  return syscall_dispatcher(SPAWN_PROCESS_SYSCALL_ENTRY, (uint64_t)name, argc,
                            argv);
}
