// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "../include/drivers/videoDriver.h"
#include "../include/lib/memory_manager.h"

#define MAX_ORDER 18
#define MIN_ORDER 3

#define BLOCKSIZE(order) (1UL << ((order) + 6))

#define GET_BUDDY(ptr, base, order)                                            \
  ((void *)((((uintptr_t)(ptr) - (uintptr_t)(base)) ^ BLOCKSIZE(order)) +      \
            (uintptr_t)(base)))

typedef struct block {
  struct block *next;
  int order;
  int is_free;
} block_t;

typedef struct memory_manager_cdt {
  void *base;
  void *end;
  block_t *free_lists[MAX_ORDER + 1];
} memory_manager_cdt;

memory_manager_adt buddy_kmm_init(void *const restrict memory_to_manage) {
  if (!memory_to_manage)
    return NULL;

  memory_manager_cdt *m = (memory_manager_cdt *)memory_to_manage;

  m->base = (void *)((uintptr_t)memory_to_manage + sizeof(memory_manager_cdt));
  m->end = (void *)((uintptr_t)m->base + HEAP_SIZE);

  for (int i = 0; i <= MAX_ORDER; i++)
    m->free_lists[i] = NULL;

  block_t *block = (block_t *)m->base;
  block->order = MAX_ORDER;
  block->is_free = 1;
  block->next = NULL;
  m->free_lists[MAX_ORDER] = block;

  return m;
}

static int get_order(size_t size) {
  size += sizeof(block_t);
  int order = MIN_ORDER;
  while (order <= MAX_ORDER && BLOCKSIZE(order) < size)
    order++;
  return order;
}

static block_t *alloc_block(memory_manager_cdt *m, int order) {
  if (order < MIN_ORDER || order > MAX_ORDER)
    return NULL;

  if (m->free_lists[order]) {
    block_t *block = m->free_lists[order];
    m->free_lists[order] = block->next;
    block->is_free = 0;
    block->next = NULL;
    return block;
  }

  block_t *big_block = alloc_block(m, order + 1);
  if (!big_block)
    return NULL;

  size_t half_size = BLOCKSIZE(order);
  block_t *buddy = (block_t *)((uintptr_t)big_block + half_size);

  buddy->order = order;
  buddy->is_free = 1;
  buddy->next = m->free_lists[order];
  m->free_lists[order] = buddy;

  big_block->order = order;
  big_block->is_free = 0;
  big_block->next = NULL;

  return big_block;
}

static inline int is_valid_address(memory_manager_cdt *m, void *addr) {
  return (addr >= m->base) && (addr < m->end);
}

static void free_block(memory_manager_cdt *m, block_t *block) {
  int order = block->order;

  if (block->is_free)
    return;

  if (order < MIN_ORDER || order > MAX_ORDER)
    return;

  block->is_free = 1;

  while (order < MAX_ORDER) {
    block_t *buddy = (block_t *)GET_BUDDY(block, m->base, order);

    if (!is_valid_address(m, buddy))
      break;

    block_t **curr = &m->free_lists[order];
    block_t *prev = NULL;
    int buddy_found = 0;

    while (*curr) {
      if (*curr == buddy) {
        buddy_found = 1;
        break;
      }
      prev = *curr;
      curr = &(*curr)->next;
    }

    if (!buddy_found || !buddy->is_free || buddy->order != order)
      break;

    if (prev)
      prev->next = buddy->next;
    else
      m->free_lists[order] = buddy->next;

    block = (block < buddy) ? block : buddy;
    block->order = order + 1;
    block->is_free = 1;
    order++;
  }

  block->next = m->free_lists[order];
  m->free_lists[order] = block;
}

void *buddy_kmalloc(memory_manager_adt const restrict m_, size_t size) {
  if (!m_ || size == 0)
    return NULL;

  memory_manager_cdt *m = (memory_manager_cdt *)m_;
  int order = get_order(size);
  block_t *block = alloc_block(m, order);

  if (!block)
    return NULL;

  return (void *)((uintptr_t)block + sizeof(block_t));
}

void buddy_kmm_free(memory_manager_adt m_, void *ptr) {
  if (!ptr || !m_)
    return;

  memory_manager_cdt *m = (memory_manager_cdt *)m_;
  block_t *block = (block_t *)((uintptr_t)ptr - sizeof(block_t));

  if ((void *)block < m->base || (void *)block >= m->end)
    return;

  if (block->is_free)
    return;

  free_block(m, block);
}

int64_t buddy_kmm_mem_info(memory_info *info, memory_manager_adt m_) {
  if (!info || !m_)
    return -1;

  memory_manager_cdt *m = (memory_manager_cdt *)m_;

  info->total_size = (uintptr_t)m->end - (uintptr_t)m->base;
  info->free = 0;

  for (int i = 0; i <= MAX_ORDER; i++) {
    block_t *curr = m->free_lists[i];
    while (curr) {
      info->free += BLOCKSIZE(i);
      curr = curr->next;
    }
  }

  info->reserved = info->total_size - info->free;

  return 0;
}

void buddy_kmm_dump_state(memory_manager_adt m_) {
  if (!m_)
    return;

  memory_manager_cdt *m = (memory_manager_cdt *)m_;

  print_str("=== Buddy Allocator Dump ===\n", 30, 0);

  // Contar memoria libre sumando bloques * tamaño bloque
  size_t free_bytes = 0;

  for (int i = MIN_ORDER; i <= MAX_ORDER; i++) {
    block_t *curr = m->free_lists[i];
    while (curr) {
      free_bytes += BLOCKSIZE(i);
      curr = curr->next;
    }
  }

  size_t total_bytes = HEAP_SIZE; // total memoria
  size_t used_bytes = total_bytes - free_bytes;

  // Para mostrar en bloques mínimos
  size_t block_min_size = BLOCKSIZE(MIN_ORDER);
  size_t total_blocks = total_bytes / block_min_size;
  size_t free_blocks = free_bytes / block_min_size;
  size_t used_blocks = used_bytes / block_min_size;

  char buffer[32];

  print_str("Total blocks: ", 15, 0);
  utoa(total_blocks, buffer, 10);
  print_str(buffer, str_len(buffer), 0);
  print_str("\n", 1, 0);

  print_str("Free blocks: ", 14, 0);
  utoa(free_blocks, buffer, 10);
  print_str(buffer, str_len(buffer), 0);
  print_str("\n", 1, 0);

  print_str("Used blocks: ", 14, 0);
  utoa(used_blocks, buffer, 10);
  print_str(buffer, str_len(buffer), 0);
  print_str("\n", 1, 0);
}
