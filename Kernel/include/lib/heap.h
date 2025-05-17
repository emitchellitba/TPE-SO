#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>

void init_heap();
void *my_malloc(size_t size);
void my_free(void *ptr);
void print_memory_state();

#endif
