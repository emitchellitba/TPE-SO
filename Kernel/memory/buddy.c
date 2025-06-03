// #ifdef BUDDY
#include "../include/lib/memory_manager.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_MEMORY (1024 * 1024 * 2) // 2 MB
#define MIN_BLOCK_SIZE 32
#define LEVELS 16
#define TREE_SIZE ((1 << (LEVELS + 1)) - 1)

#define GET_LEFT_CHILD(i) (2 * (i) + 1)
#define GET_RIGHT_CHILD(i) (2 * (i) + 2)
#define GET_PARENT(i) (((i) - 1) / 2)
#define GET_SIBLING(i) (((i) % 2) ? ((i) + 1) : ((i) - 1)) // Funciona si i > 0

typedef struct {
  char tree[TREE_SIZE]; // 0: libre, 1: ocupado/dividido
  uint8_t *heap;        // inicio del heap
  uint64_t free_mem;
} memory_manager_cdt;

static int get_level_for_index(int index) {
  int level = 0;
  if (index < 0)
    return -1;
  int temp_idx = index + 1;
  int lvl = -1;
  while (temp_idx > 0) {
    temp_idx >>= 1;
    lvl++;
  }
  return lvl;
}

static int get_level(int index) {
  int level = 0, count = 0;
  while (count + (1 << level) <= index) {
    count += (1 << level);
    level++;
  }
  return level;
}

static uint64_t get_block_size_for_level(int level) {
  if (level < 0 || level > LEVELS)
    return 0;
  return MAX_MEMORY >> level;
}

static void mark_free_and_merge(memory_manager_cdt *mem, int index, int level) {
  mem->tree[index] = 0; // Marcar el nodo actual como libre

  if (level ==
      0) { // Si estamos en la raíz, no hay más para fusionar hacia arriba
    return;
  }

  int sibling = GET_SIBLING(index);

  if (index > 0 && sibling < TREE_SIZE && mem->tree[sibling] == 0) {
    int parent = GET_PARENT(index);
    if (parent >= 0) {
      mark_free_and_merge(mem, parent, level - 1);
    }
  }
}

static int find_block_recursive(memory_manager_cdt *mem, int current_node_index,
                                int current_level, int requested_level) {

  if (current_node_index >= TREE_SIZE) {
    return -1;
  }

  if (mem->tree[current_node_index] == 1) {

    if (current_level == requested_level)
      return -1;
  }

  if (current_level == requested_level && mem->tree[current_node_index] == 0) {
    mem->tree[current_node_index] = 1; // Marcar como ocupado
    return current_node_index;
  }

  if (current_level < requested_level) {
    int left_child_idx = GET_LEFT_CHILD(current_node_index);
    int found_node = find_block_recursive(mem, left_child_idx,
                                          current_level + 1, requested_level);
    if (found_node != -1) {
      mem->tree[current_node_index] =
          1; // Marcar el padre como dividido/ocupado
      return found_node;
    }

    int right_child_idx = GET_RIGHT_CHILD(current_node_index);
    found_node = find_block_recursive(mem, right_child_idx, current_level + 1,
                                      requested_level);
    if (found_node != -1) {
      mem->tree[current_node_index] =
          1; // Marcar el padre como dividido/ocupado
      return found_node;
    }
  }

  return -1; // No se encontró un bloque adecuado
}

memory_manager_adt buddy_kmm_init(void *const restrict p) {
  memory_manager_cdt *mem = (memory_manager_cdt *)p;

  mem->heap = (uint8_t *)p + sizeof(memory_manager_cdt);
  for (int i = 0; i < TREE_SIZE; i++) {
    mem->tree[i] = 0; // 0 significa libre
  }
  mem->free_mem = MAX_MEMORY;

  return mem;
}

void *buddy_kmalloc(memory_manager_adt const restrict m, const size_t size) {
  memory_manager_cdt *mem = (memory_manager_cdt *)m;
  if (mem == NULL || size == 0)
    return NULL;

  // Calcular el tamaño de bloque de buddy y el nivel necesarios
  uint64_t actual_alloc_size = MIN_BLOCK_SIZE;
  int target_alloc_level =
      LEVELS; // Nivel más profundo (16) para MIN_BLOCK_SIZE

  while (actual_alloc_size < size) {
    if (target_alloc_level ==
        0) {       // Ya estamos en MAX_MEMORY y sigue siendo pequeño
      return NULL; // El tamaño solicitado es mayor que MAX_MEMORY
    }
    actual_alloc_size <<= 1;
    target_alloc_level--;
  }

  if (mem->free_mem < actual_alloc_size) {
    return NULL;
  }

  int allocated_node_index =
      find_block_recursive(mem, 0, 0, target_alloc_level);

  if (allocated_node_index == -1) {
    return NULL;
  }

  // Calcular el offset del bloque asignado dentro del heap
  // k = index_del_nodo - ((1 << nivel_del_nodo) - 1)
  uint64_t block_k_in_level =
      allocated_node_index - ((1 << target_alloc_level) - 1);
  uint64_t offset = block_k_in_level * actual_alloc_size;

  mem->free_mem -= actual_alloc_size;

  return (void *)(mem->heap + offset);
}

void buddy_kmm_free(void *ptr, memory_manager_adt m) {
  memory_manager_cdt *mem = (memory_manager_cdt *)m;
  if (mem == NULL || ptr == NULL)
    return;

  uint8_t *ptr_addr = (uint8_t *)ptr;
  if (ptr_addr < mem->heap || ptr_addr >= (void *)(mem->heap + MAX_MEMORY)) {
    // Fuera de los límites del heap
    return;
  }

  uint64_t offset_in_heap = ptr_addr - mem->heap;

  // Encontrar el nodo exacto (índice y nivel) que fue asignado para este
  // puntero. Esto requiere navegar por el árbol, ya que el tamaño no se
  // almacena.
  int node_to_free_idx = 0;
  int level_of_node_to_free = 0;
  uint64_t current_block_size = MAX_MEMORY;
  uint64_t current_node_offset =
      0; // Offset del nodo actual que estamos inspeccionando

  while (level_of_node_to_free <= LEVELS) {
    if (node_to_free_idx >= TREE_SIZE)
      return; // Índice fuera de rango, error

    if (mem->tree[node_to_free_idx] == 0 && level_of_node_to_free > 0) {

      return;
    }

    int left_child = GET_LEFT_CHILD(node_to_free_idx);
    int right_child = GET_RIGHT_CHILD(node_to_free_idx);

    int is_leaf_allocation = 1; // Asumir que es hoja de asignación
    if (level_of_node_to_free <
        LEVELS) { // Si no es el nivel más profundo, verificar hijos
      if (left_child < TREE_SIZE && mem->tree[left_child] == 1) {
        is_leaf_allocation = 0;
      }
      if (right_child < TREE_SIZE && mem->tree[right_child] == 1 &&
          is_leaf_allocation == 0) {

      } else if (right_child < TREE_SIZE && mem->tree[right_child] == 1) {
        is_leaf_allocation = 0;
      }
    }

    int is_actual_block = 0;
    if (mem->tree[node_to_free_idx] == 1) {
      if (level_of_node_to_free == LEVELS) {
        is_actual_block = 1;
      } else {
        int lc_free_or_nonexistent =
            (left_child >= TREE_SIZE || mem->tree[left_child] == 0);
        int rc_free_or_nonexistent =
            (right_child >= TREE_SIZE || mem->tree[right_child] == 0);
        if (lc_free_or_nonexistent && rc_free_or_nonexistent) {
          is_actual_block = 1;
        }
      }
    }

    if (is_actual_block && current_node_offset == offset_in_heap) {
      // Encontramos el bloque exacto que fue asignado.
      break;
    }

    if (level_of_node_to_free == LEVELS && !is_actual_block) {
      // Llegamos al nivel más profundo pero no encontramos el bloque exacto
      return;
    }
    if (level_of_node_to_free >= LEVELS)
      break; // Evitar ir más allá

    // Descender al hijo apropiado
    current_block_size >>= 1; // Tamaño de los bloques hijos
    if (offset_in_heap < current_node_offset + current_block_size) {
      // El offset está en el hijo izquierdo
      node_to_free_idx = left_child;
      // current_node_offset se mantiene para el hijo izquierdo
    } else {
      // El offset está en el hijo derecho
      node_to_free_idx = right_child;
      current_node_offset +=
          current_block_size; // Ajustar offset base para el hijo derecho
    }
    level_of_node_to_free++;
  }

  if (level_of_node_to_free > LEVELS || mem->tree[node_to_free_idx] == 0) {
    // No se encontró un bloque ocupado que coincida o ya estaba libre
    return;
  }

  uint64_t size_of_freed_block =
      get_block_size_for_level(level_of_node_to_free);

  mem->free_mem += size_of_freed_block;
  mark_free_and_merge(mem, node_to_free_idx, level_of_node_to_free);
}

int64_t buddy_kmm_mem_info(memory_info *info, memory_manager_adt m) {
  if (info == NULL || m == NULL)
    return -1;

  memory_manager_cdt *mem = (memory_manager_cdt *)m;
  info->total_size = MAX_MEMORY;
  info->free = mem->free_mem;
  return 0;
}

void buddy_kmm_dump_state(memory_manager_adt m) {
  memory_manager_cdt *mem_cdt = (memory_manager_cdt *)m;
  if (!mem_cdt)
    return;

  printf("Buddy Allocator State:\n");
  printf("  Total Memory: %lu\n", (unsigned long)MAX_MEMORY);
  printf("  Free Memory:  %lu\n", (unsigned long)mem_cdt->free_mem);
  printf("  Tree (0: free, 1: occupied/divided):\n");

  int current_index = 0;
  for (int l = 0; l <= LEVELS; ++l) {
    if (current_index >= TREE_SIZE)
      break;
    printf("  Level %2d (size %8lu): ", l,
           (unsigned long)get_block_size_for_level(l));
    int nodes_in_level = 1 << l;
    for (int n = 0; n < nodes_in_level; ++n) {
      if (current_index >= TREE_SIZE)
        break;
      printf("%d ", mem_cdt->tree[current_index]);
      current_index++;
    }
    printf("\n");
  }
  printf("----------------------------------------\n");
}

// #endif

// Buddy Allocator: Resumen del Funcionamiento

// El Buddy Allocator gestiona la memoria dividiéndola en bloques de tamaños que
// son potencias de dos, usando un árbol implícito para rastrear su estado.

// Estructura (memory_manager_cdt):

// tree: Un array que representa un árbol binario. Cada nodo (índice)
// corresponde a un bloque de memoria. 0: Bloque libre. 1: Bloque ocupado o
// dividido en bloques más pequeños. heap: Puntero al inicio de la memoria
// gestionada. free_mem: Total de memoria libre. Inicialización
// (buddy_kmm_init):

// Todo el tree se marca como 0 (libre), representando la memoria total como un
// único gran bloque libre. free_mem se establece a MAX_MEMORY.

// Inicialización (buddy_kmm_init):

// Todo el tree se marca como 0 (libre), representando la memoria total como un
// único gran bloque libre. free_mem se establece a MAX_MEMORY.

// Asignación (buddy_kmalloc):

// Calcular Tamaño: Se determina el menor tamaño de bloque potencia de dos
// (actual_alloc_size) que satisface la petición y su nivel (target_alloc_level)
// en el árbol. Buscar Bloque (find_block_recursive): Se busca recursivamente
// desde la raíz (nivel 0). Si se encuentra un bloque libre (0) del tamaño
// exacto (target_alloc_level), se marca como 1 (ocupado). Si se debe descender
// (porque el bloque actual es más grande o está libre pero se necesita uno más
// pequeño), se exploran los hijos. Importante: Cuando un bloque hijo se asigna,
// todos sus nodos ancestros en el camino de la recursión se marcan como 1
// (dividido/ocupado). Retorno: Si se encuentra un bloque, se calcula su
// dirección en el heap y se devuelve. Si no, NULL.

// Liberación (buddy_kmm_free):

// Localizar Bloque: Se navega por el tree (desde la raíz) usando el offset del
// puntero liberado para encontrar el nodo exacto (node_to_free_idx) y su nivel
// (level_of_node_to_free) que fue originalmente asignado. Se busca un nodo
// marcado como 1 que sea una "hoja de la asignación" (no esté marcado 1 solo
// porque sus hijos lo están). Marcar y Fusionar (mark_free_and_merge): El nodo
// encontrado se marca como 0 (libre). Se comprueba su "buddy" (hermano en el
// árbol). Si el buddy también está libre (0): Los dos bloques se fusionan. Su
// nodo padre (que antes estaba 1 por estar dividido) se marca ahora como 0
// (libre). Este proceso de fusión se repite recursivamente hacia arriba en el
// árbol mientras se encuentren buddies libres.

// Objetivo Principal: Minimizar la fragmentación externa intentando siempre
// fusionar bloques libres adyacentes (buddies) para formar bloques más grandes.
// La fragmentación interna ocurre porque se asigna un bloque potencia de dos
// que podría ser más grande que la solicitud exacta.