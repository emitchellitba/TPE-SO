#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

#include <stdint.h>
#include <stdlib.h>

typedef struct MemoryManagerCDT *MemoryManagerADT;

/*
 * This function initializes the memory manager. It takes one pointer:
 * - memory_to_manage: a pointer to the memory that will be managed
 *
 * Returns a pointer to the initialized memory manager.
 */
MemoryManagerADT kmm_init(void *const restrict memory_to_manage);

/*
 * This function allocates memory of the specified size from the memory manager.
 * - memory_manager: a pointer to the memory manager
 * - to_allocate: the size of memory to allocate (in bytes)
 *
 * Returns a pointer to the allocated memory.
 */
void *kmalloc(MemoryManagerADT const restrict memory_manager,
              const size_t to_allocate);

#define HEAP_SIZE 0x1000000 // 16MB

#endif