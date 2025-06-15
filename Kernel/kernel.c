// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <kernel.h>

int64_t syscall_dispatcher(uint64_t rax, ...);
extern void initialize_idle(void);

memory_manager_adt kernel_mem;

static int kernel_log_level = LOG_DEBUG;
LOGGER_DEFINE(kernel, kernel_log, kernel_log_level)

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;
static void *const heapModuleAddress = (void *)0x600000;

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize) {
  memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
  return (void *)((uint64_t)&endOfKernel +
                  PageSize * 8       // The size of the stack itself, 32KiB
                  - sizeof(uint64_t) // Begin at the top of the stack
  );
}

void *initializeKernelBinary() {
  void *moduleAddresses[] = {sampleCodeModuleAddress, sampleDataModuleAddress};

  loadModules(&endOfKernelBinary, moduleAddresses);

  clearBSS(&bss, &endOfKernel - &bss);

  return getStackBase();
}

int main() {
  load_idt();

  my_sem_init();
  kernel_mem = kmm_init(heapModuleAddress);

#if defined(USE_BUDDY_MM)
  kernel_log(LOG_INFO, "Buddy memory manager initialized\n");
#else
  kernel_log(LOG_INFO, "Simple memory manager initialized\n");
#endif

  _cli();

  initialize_scheduler();

  initialize_idle();

  my_sem_init();

  char *name = "init";
  fs_load(name, (fs_entry_point_t)sampleCodeModuleAddress);

  char *argv[] = {name, NULL};
  int argc = 1;

  syscall_dispatcher(SPAWN_PROCESS_SYSCALL_ENTRY, (uint64_t)name, argc, argv);

  call_timer_tick();

  return 0;
}
