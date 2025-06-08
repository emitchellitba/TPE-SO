#include "memory_manager.h"
#include <stddef.h>
#include <stdint.h>
#include <unistd.h> // syscall write

#define MIN_ORDER 5  // 2^5 = 32 bytes mínimo
#define MAX_ORDER 24 // 2^24 = 16MB
#define NUM_ORDERS (MAX_ORDER - MIN_ORDER + 1)

typedef struct Block {
  struct Block *next;
} Block;

typedef struct memory_manager_cdt {
  Block *free_lists[NUM_ORDERS];
  uint8_t *base;
} memory_manager_cdt;

static inline size_t order_to_size(int order) {
  return 1UL << (order + MIN_ORDER);
}

static inline int size_to_order(size_t size) {
  size += (1UL << MIN_ORDER) - 1;
  int order = 0;
  size >>= MIN_ORDER;
  while (size >>= 1)
    order++;
  return order;
}

static inline uintptr_t align_up(uintptr_t addr, size_t align) {
  return (addr + align - 1) & ~(align - 1);
}

static inline Block *buddy_of(Block *block, int order, uint8_t *base) {
  uintptr_t offset = (uintptr_t)block - (uintptr_t)base;
  uintptr_t buddy_offset = offset ^ order_to_size(order);
  return (Block *)(base + buddy_offset);
}

memory_manager_adt buddy_kmm_init(void *const restrict memory_to_manage) {
  memory_manager_adt manager = (memory_manager_adt)memory_to_manage;
  uintptr_t base_addr =
      (uintptr_t)memory_to_manage + sizeof(memory_manager_cdt);
  base_addr = align_up(base_addr, 1UL << MIN_ORDER);
  manager->base = (uint8_t *)base_addr;

  for (int i = 0; i < NUM_ORDERS; i++) {
    manager->free_lists[i] = NULL;
  }

  Block *initial_block = (Block *)manager->base;
  initial_block->next = NULL;
  manager->free_lists[NUM_ORDERS - 1] = initial_block;

  return manager;
}

void *buddy_kmalloc(memory_manager_adt const restrict mem, const size_t size) {
  if (!size)
    return NULL;

  size_t required = size + sizeof(size_t); // Save order before user data
  int order = size_to_order(required);
  if (order >= NUM_ORDERS)
    return NULL;

  int current = order;
  while (current < NUM_ORDERS && mem->free_lists[current] == NULL) {
    current++;
  }

  if (current == NUM_ORDERS)
    return NULL;

  while (current > order) {
    Block *split = mem->free_lists[current];
    mem->free_lists[current] = split->next;

    current--;
    size_t block_size = order_to_size(current);
    Block *buddy = (Block *)((uint8_t *)split + block_size);
    buddy->next = NULL;

    split->next = NULL;
    mem->free_lists[current] = split;
    mem->free_lists[current]->next = buddy;
  }

  Block *block = mem->free_lists[order];
  mem->free_lists[order] = block->next;

  *((size_t *)block) = order;
  return (uint8_t *)block + sizeof(size_t);
}

void buddy_kmm_free(void *ptr, memory_manager_adt mem) {
  if (!ptr)
    return;

  uint8_t *block_ptr = (uint8_t *)ptr - sizeof(size_t);
  int order = *((size_t *)block_ptr);

  Block *block = (Block *)block_ptr;
  block->next = NULL;

  while (order < NUM_ORDERS) {
    Block *buddy = buddy_of(block, order, mem->base);
    Block **curr = &mem->free_lists[order];
    Block *prev = NULL;

    while (*curr && *curr != buddy) {
      prev = *curr;
      curr = &(*curr)->next;
    }

    if (*curr == buddy) {
      if (prev)
        prev->next = buddy->next;
      else
        mem->free_lists[order] = buddy->next;

      if ((uintptr_t)block > (uintptr_t)buddy) {
        Block *tmp = block;
        block = buddy;
        buddy = tmp;
      }

      order++;
    } else {
      break;
    }
  }

  block->next = mem->free_lists[order];
  mem->free_lists[order] = block;
}

int64_t buddy_kmm_mem_info(memory_info *info, memory_manager_adt mem) {
  if (!info)
    return -1;

  uint64_t total = 0, free = 0;
  for (int i = 0; i < NUM_ORDERS; i++) {
    size_t block_size = order_to_size(i);
    for (Block *b = mem->free_lists[i]; b; b = b->next) {
      free += block_size;
    }
    total += block_size * (1UL << (NUM_ORDERS - i - 1)); // estimation
  }

  info->total_size = HEAP_SIZE;
  info->free = free;
  return 0;
}

void buddy_kmm_dump_state(memory_manager_adt mem) {
  // char buffer[128];
  // const char *prefix = "Buddy Dump:\n";
  // write(1, prefix, sizeof("Buddy Dump:\n") - 1);

  // for (int i = 0; i < NUM_ORDERS; i++) {
  //     int count = 0;
  //     for (Block *b = mem->free_lists[i]; b; b = b->next) {
  //         count++;
  //     }

  //     int len = 0;
  //     size_t block_size = order_to_size(i);
  //     len = snprintf(buffer, sizeof(buffer), "Order %d (size %zu): %d
  //     blocks\n", i, block_size, count); write(1, buffer, len);
  // }
}
