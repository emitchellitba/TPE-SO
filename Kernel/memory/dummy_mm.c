#include <memory_manager.h>

// In this memory manager, a block is a byte of memory (char).

typedef struct memory_manager_cdt {
  char *next_address;
} memory_manager_cdt;

memory_manager_adt dummy_kmm_init(void *const restrict memory_to_manage) {
  memory_manager_cdt *aux = (memory_manager_cdt *)memory_to_manage;

  aux->next_address = (char *)memory_to_manage;
  kmalloc((memory_manager_adt)aux,
          sizeof(memory_manager_cdt)); // reserves the space for the CDT

  return (memory_manager_adt)aux;
}

void *dummy_kmalloc(memory_manager_adt const restrict memory_manager,
                    const size_t to_allocate) {
  void *ptr = (void *)(memory_manager->next_address);

  if (ptr == NULL || to_allocate == 0) {
    return NULL;
  }

  memory_manager->next_address += to_allocate;
  return ptr;
}

void dummy_kmm_free(void *ptr, memory_manager_adt m) {
  // This function is not implemented in this dummy memory manager.
  // In a real memory manager, it would free the allocated memory.
  return;
}

int64_t dummy_kmm_mem_info(memory_info *info, memory_manager_adt mem) {}

void dummy_kmm_dump_state(memory_manager_adt mem) {
  // This function is not implemented in this dummy memory manager.
  // In a real memory manager, it would print the state of the memory.
  return;
}
