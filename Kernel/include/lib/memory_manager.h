#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>

#define HEAP_SIZE 16777216UL

#define HEAP_BASE 0x00600000

// Opaque pointer para el manejador de memoria
typedef struct memory_manager_cdt *memory_manager_adt;

// Estructura opcional para información de memoria
typedef struct {
  uint64_t total_size;
  uint64_t free;
  uint64_t reserved;
} memory_info;

#if defined(USE_BUDDY_MM)

/* === Buddy Memory Manager === */

// Inicializa el manejador buddy sobre el bloque dado
memory_manager_adt buddy_kmm_init(void *const restrict memory_to_manage);

// Asigna un bloque de memoria del tamaño solicitado
void *buddy_kmalloc(memory_manager_adt const restrict memory_manager,
                    const size_t size);

// Libera una dirección previamente asignada
void buddy_kmm_free(void *ptr, memory_manager_adt mem);

// Devuelve la cantidad de memoria libre y total (opcional)
int64_t buddy_kmm_mem_info(memory_info *info, memory_manager_adt mem);

// Muestra el estado actual del árbol buddy
void buddy_kmm_dump_state(memory_manager_adt mem);

// Alias genéricos para usar el mismo nombre en ambas implementaciones
#define kmm_init buddy_kmm_init
#define kmalloc buddy_kmalloc
#define kmm_free buddy_kmm_free
#define kmm_dump_state buddy_kmm_dump_state
#define kmm_mem_info buddy_kmm_mem_info

#else // Si no se define USE_BUDDY_MM, se usa el manejador simple siempre

/* === Simple Memory Manager === */

// Inicializa el manejador simple sobre el bloque dado
memory_manager_adt dummy_kmm_init(void *restrict memory_to_manage);

// Asigna un bloque de memoria del tamaño solicitado
void *my_kmalloc(memory_manager_adt restrict memory_manager, size_t size);

// Libera una dirección previamente asignada
void my_kmm_free(void *ptr, memory_manager_adt mem);

// Devuelve la cantidad de memoria libre y total (opcional)
int64_t my_kmm_mem_info(memory_info *info, memory_manager_adt mem);

// Muestra el estado actual de la memoria simple
void my_kmm_dump_state(memory_manager_adt mem);

// Alias genéricos para usar el mismo nombre en ambas implementaciones
#define kmm_init my_kmm_init
#define kmalloc my_kmalloc
#define kmm_free my_kmm_free
#define kmm_dump_state my_kmm_dump_state
#define kmm_mem_info my_kmm_mem_info

#endif

#define KMEMORY_DECLARE extern memory_manager_adt kernel_mem;

#endif // MEMORY_MANAGER_H
