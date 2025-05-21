#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

#define HEAP_SIZE 0x1000000 // 16MB

typedef struct MemoryManagerCDT *MemoryManagerADT;

#if defined(USE_BUDDY_MM)

// Advanced memory manager declarations
MemoryManagerADT buddy_kmm_init(void *const restrict memory_to_manage);
void *buddy_kmalloc(MemoryManagerADT const restrict memory_manager,
                    const size_t to_allocate);
void buddy_kmm_free(void *ptr);
void buddy_kmm_dump_state(void);

// Optionally, alias the advanced functions to the generic names:
#define kmm_init buddy_kmm_init
#define kmalloc buddy_kmalloc
#define kmm_free buddy_kmm_free
#define kmm_dump_state buddy_kmm_dump_state

#else

// Simple memory manager declarations
MemoryManagerADT kmm_init(void *const restrict memory_to_manage);
void *kmalloc(MemoryManagerADT const restrict memory_manager,
              const size_t to_allocate);
void kmm_free(void *ptr);
void kmm_dump_state(void);

#endif

#define KMEMORY_DECLARE extern MemoryManagerADT kernel_mem;

#endif // MEMORY_MANAGER_H
