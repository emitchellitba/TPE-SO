#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

#define HEAP_SIZE 0x1000000 // 16MB

typedef struct MemoryManagerCDT *MemoryManagerADT;

#if defined(USE_SIMPLE_MM)

// Simple memory manager declarations
MemoryManagerADT kmm_init(void *const restrict memory_to_manage);
void *kmalloc(MemoryManagerADT const restrict memory_manager,
              const size_t to_allocate);
void kmm_free(void *ptr);
void kmm_dump_state(void);

#elif defined(USE_BUDDY_MM)

// Advanced memory manager declarations
MemoryManagerADT buddy_kmm_init(void *const restrict memory_to_manage);
void *buddy_kmalloc(MemoryManagerADT const restrict memory_manager,
                    const size_t to_allocate);
void buddy_kmm_free(void *ptr);
void buddy_kmm_dump_state(void);

// Optionally, alias the advanced functions to the generic names:
#define kmm_init adv_kmm_init
#define kmalloc adv_kmalloc
#define kmm_free adv_kmm_free
#define kmm_dump_state adv_kmm_dump_state

#endif

#endif // MEMORY_MANAGER_H
