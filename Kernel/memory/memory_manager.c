#include "memory_manager.h"
#include "heap.h"

void mm_init() { init_heap(); }

void *mm_alloc(size_t size) { return my_malloc(size); }

void mm_free(void *ptr) { my_free(ptr); }

// Imprime el estado actual del heap
void mm_dump_state() { print_memory_state(); }
