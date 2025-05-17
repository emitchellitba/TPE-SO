#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

void mm_init();
void *mm_alloc(size_t size);
void mm_free(void *ptr);
void mm_dump_state();

#endif
