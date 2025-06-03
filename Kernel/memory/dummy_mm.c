#include <stdint.h>
#include <stdlib.h> // Podría ser redundante si memory_manager.h incluye stddef.h (que define NULL y size_t)

#include "../include/lib/memory_manager.h" // <--- Añade esta línea
#include "../include/naiveConsole.h"

#define MANAGED_MEMORY_SIZE 20480 // 20KB

// In this memory manager, a block is a byte of memory (char).

typedef struct memory_manager_cdt {
  char *next_address;
  char *end_address; // Add end address to check boundaries
  size_t total_size; // Add total size for info
  size_t used_size;  // Add used size for info
} memory_manager_cdt;

memory_manager_adt dummy_kmm_init(void *memory_to_manage) {
  if (memory_to_manage == NULL) {
    return NULL;
  }

  memory_manager_cdt *aux = (memory_manager_cdt *)memory_to_manage;

  // Initialize the next_address to point after the CDT structure
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

  // Check if we have enough space
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
  // In a dummy memory manager, we don't actually free memory
  // But we can validate the pointer
  if (ptr == NULL || m == NULL) {
    return;
  }

  // Check if pointer is within our managed memory range
  char *start = (char *)m + sizeof(memory_manager_cdt);
  if (ptr < (void *)start || ptr >= (void *)m->end_address) {
    return; // Invalid pointer
  }
}

int64_t dummy_kmm_mem_info(memory_info *info, memory_manager_adt mem) {
  if (info == NULL || mem == NULL) {
    return -1;
  }

  info->total_size = mem->total_size; // Corregido: usar total_size
  // El campo 'used_memory' no existe en la definición de 'memory_info'.
  // El campo 'free' se calcula y se asigna.
  info->free = mem->total_size - mem->used_size; // Corregido: usar free

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
    ncPrintDec(info.total_size); // Corregido: usar info.total_size
    ncPrint(" bytes\n");
    ncPrint("Used Memory: ");
    // Para mostrar la memoria usada, puedes calcularla aquí si es necesario,
    // ya que no es parte directa de la estructura 'memory_info'.
    ncPrintDec(mem->used_size); // O imprimir directamente desde 'mem' si es lo
                                // que quieres mostrar
    // O calcularla: ncPrintDec(info.total_size - info.free);
    ncPrint(" bytes\n");
    ncPrint("Free Memory: ");
    ncPrintDec(info.free); // Corregido: usar info.free
    ncPrint(" bytes\n");
    ncPrint("===============================\n");
  }
}
