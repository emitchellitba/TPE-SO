#include "dummy_mm.h"

// In this memory manager, a block is a byte of memory (char).

typedef struct MemoryManagerCDT {
  char *next_address;
} MemoryManagerCDT;

MemoryManagerADT kmm_init(void *const restrict memory_to_manage) {
  MemoryManagerCDT *aux = (MemoryManagerCDT *)memory_to_manage;

  aux->next_address = (char *)memory_to_manage;
  kmalloc((MemoryManagerADT)aux,
          sizeof(MemoryManagerCDT)); // reserves the space for the CDT

  return (MemoryManagerADT)aux;
}

void *kmalloc(MemoryManagerADT const restrict memory_manager,
              const size_t to_allocate) {
  void *ptr = (void *)(memory_manager->next_address);

  if (ptr == NULL || to_allocate == 0) {
    return NULL;
  }

  memory_manager->next_address += to_allocate;
  return ptr;
}
