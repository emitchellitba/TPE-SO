// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <stdint.h>
#include <stdlib.h>

#include "../include/lib/memory_manager.h"
#include "../include/naiveConsole.h"

#define MANAGED_MEMORY_SIZE 1048576

typedef struct memory_manager_cdt {
  char *next_address;
  char *end_address;
  size_t total_size;
  size_t used_size;
} memory_manager_cdt;

memory_manager_adt dummy_kmm_init(void *memory_to_manage) {
  if (memory_to_manage == NULL) {
    return NULL;
  }

  memory_manager_cdt *aux = (memory_manager_cdt *)memory_to_manage;

  aux->next_address = (char *)memory_to_manage + sizeof(memory_manager_cdt);
  aux->end_address = (char *)memory_to_manage + MANAGED_MEMORY_SIZE;
  aux->total_size = MANAGED_MEMORY_SIZE - sizeof(memory_manager_cdt);
  aux->used_size = 0;

  return (memory_manager_adt)aux;
}

void *dummy_kmalloc(memory_manager_adt memory_manager,
                    const size_t to_allocate) {
  if (memory_manager == NULL || to_allocate == 0) {
    return NULL;
  }

  if (memory_manager->next_address + to_allocate >
      memory_manager->end_address) {
    return NULL;
  }

  void *ptr = (void *)(memory_manager->next_address);
  memory_manager->next_address += to_allocate;
  memory_manager->used_size += to_allocate;

  return ptr;
}

void dummy_kmm_free(void *ptr, memory_manager_adt m) {
  if (ptr == NULL || m == NULL) {
    return;
  }

  char *start = (char *)m + sizeof(memory_manager_cdt);
  if (ptr < (void *)start || ptr >= (void *)m->end_address) {
    return;
  }
}

int64_t dummy_kmm_mem_info(memory_info *info, memory_manager_adt mem) {
  if (info == NULL || mem == NULL) {
    return -1;
  }

  info->total_size = mem->total_size;

  info->free = mem->total_size - mem->used_size;

  return 0;
}

void dummy_kmm_dump_state(memory_manager_adt mem) {
  if (mem == NULL) {
    return;
  }

  memory_info info;
  if (dummy_kmm_mem_info(&info, mem) == 0) {
    ncPrint("\n=== Dummy Memory Manager State ===\n");
    ncPrint("Total Memory: ");
    ncPrintDec(info.total_size);
    ncPrint(" bytes\n");
    ncPrint("Used Memory: ");

    ncPrintDec(mem->used_size);

    ncPrint(" bytes\n");
    ncPrint("Free Memory: ");
    ncPrintDec(info.free);
    ncPrint(" bytes\n");
    ncPrint("===============================\n");
  }
}
