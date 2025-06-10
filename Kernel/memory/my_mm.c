// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "../include/drivers/videoDriver.h"
#include "../include/lib/memory_manager.h"

typedef struct memory_manager_cdt {
  char *next_address;
  char *end_address;
  size_t total_size;
  size_t used_size;
} memory_manager_cdt;

memory_manager_adt my_kmm_init(void *memory_to_manage) {
  if (memory_to_manage == NULL) {
    return NULL;
  }

  memory_manager_cdt *aux = (memory_manager_cdt *)memory_to_manage;

  aux->next_address = (char *)memory_to_manage + sizeof(memory_manager_cdt);
  aux->end_address = (char *)memory_to_manage + HEAP_SIZE;
  aux->total_size = HEAP_SIZE - sizeof(memory_manager_cdt);
  aux->used_size = 0;

  return (memory_manager_adt)aux;
}

void *my_kmalloc(memory_manager_adt memory_manager, const size_t to_allocate) {
  if (memory_manager == NULL || to_allocate == 0) {
    return NULL;
  }

  // Validación de overflow o desbordamiento
  if (memory_manager->next_address + to_allocate >
      memory_manager->end_address) {
    return NULL;
  }

  void *ptr = (void *)(memory_manager->next_address);
  memory_manager->next_address += to_allocate;
  memory_manager->used_size += to_allocate;

  return ptr;
}

void my_kmm_free(void *ptr, memory_manager_adt m) {
  // Este allocator no soporta free real, pero validamos parámetros

  if (ptr == NULL || m == NULL) {
    return;
  }

  // Validamos que el puntero esté dentro del heap manejado
  char *start = (char *)m + sizeof(memory_manager_cdt);
  if (ptr < (void *)start || ptr >= (void *)m->end_address) {
    return;
  }

  // No hacemos nada porque no hay soporte de free,
  // pero queda validación para evitar errores
}

int64_t my_kmm_mem_info(memory_info *info, memory_manager_adt mem) {
  if (info == NULL || mem == NULL) {
    return -1;
  }

  info->total_size = mem->total_size;
  info->free = mem->total_size - mem->used_size;

  return 0;
}
void my_kmm_dump_state(memory_manager_adt mem) {
  if (mem == NULL) {
    return;
  }

  memory_info info;
  if (my_kmm_mem_info(&info, mem) == 0) {
    char buf[32];

    char *msg = "\n=== my Memory Manager State ===\n";
    print_str(msg, str_len(msg), 0);

    print_str("Total Memory: ", 14, 0);
    utoa(info.total_size, buf, 10);
    print_str(buf, str_len(buf), 0);
    print_str(" bytes\n", 7, 0);

    print_str("Used Memory: ", 13, 0);
    utoa(mem->used_size, buf, 10);
    print_str(buf, str_len(buf), 0);
    print_str(" bytes\n", 7, 0);

    print_str("Free Memory: ", 13, 0);
    utoa(info.free, buf, 10);
    print_str(buf, str_len(buf), 0);
    print_str(" bytes\n", 7, 0);

    print_str("===============================\n", 32, 0);
  }
}
