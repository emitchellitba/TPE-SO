#include <stdint.h>
#include <syscall.h>

int64_t my_getpid() { return 0; }

int64_t my_create_process(char *name, uint64_t argc, char *argv[]) { return 0; }

int64_t my_nice(uint64_t pid, uint64_t newPrio) { return 0; }

int64_t my_kill(uint64_t pid) { return 0; }

int64_t my_block(uint64_t pid) { return 0; }

int64_t my_unblock(uint64_t pid) { return 0; }

int64_t my_yield() { return 0; }

int64_t my_wait(int64_t pid) { return 0; }