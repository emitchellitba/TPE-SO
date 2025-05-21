// #ifdef BUDDY
#include "../include/lib/memory_manager.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_MEMORY (1024 * 1024 * 2) // 2 MB
#define MIN_BLOCK_SIZE 32
#define LEVELS 16
#define TREE_SIZE ((1 << LEVELS) - 1)

#define GET_LEFT_CHILD(i) (2 * (i) + 1)
#define GET_RIGHT_CHILD(i) (2 * (i) + 2)
#define GET_PARENT(i) (((i) - 1) / 2)
#define GET_SIBLING(i) (((i) % 2) ? ((i) + 1) : ((i) - 1))

typedef struct {
  char tree[TREE_SIZE]; // 0: libre, 1: ocupado
  uint8_t *heap;        // inicio del heap
  uint64_t free_mem;
} memory_manager_cdt;

static int get_level(int index) {
  int level = 0, count = 0;
  while (count + (1 << level) <= index) {
    count += (1 << level);
    level++;
  }
  return level;
}

static uint64_t get_block_size(int level) { return MAX_MEMORY >> level; }

static void mark_free(memory_manager_cdt *mem, int index, int level) {
  mem->tree[index] = 0;

  int sibling = GET_SIBLING(index);
  if (sibling < TREE_SIZE && mem->tree[sibling] == 0) {
    int parent = GET_PARENT(index);
    mark_free(mem, parent, level - 1);
  }
}

static int find_block(memory_manager_cdt *mem, int index, int level,
                      int req_level) {
  if (index >= TREE_SIZE || mem->tree[index]) {
    return -1;
  }

  if (level == req_level) {
    mem->tree[index] = 1;
    return index;
  }

  int left = find_block(mem, GET_LEFT_CHILD(index), level + 1, req_level);
  if (left != -1) {
    mem->tree[index] = 1;
    return left;
  }

  int right = find_block(mem, GET_RIGHT_CHILD(index), level + 1, req_level);
  if (right != -1) {
    mem->tree[index] = 1;
    return right;
  }

  return -1;
}

memory_manager_adt buddy_kmm_init(void *const restrict p) {
  memory_manager_cdt *mem = (memory_manager_cdt *)p;

  mem->heap = (uint8_t *)p + sizeof(memory_manager_cdt);
  for (int i = 0; i < TREE_SIZE; i++) {
    mem->tree[i] = 0;
  }
  mem->free_mem = MAX_MEMORY;

  return mem;
}

void *buddy_kmalloc(memory_manager_adt const restrict m, const size_t size) {
  memory_manager_cdt *mem = (memory_manager_cdt *)m;
  if (mem == NULL || size == 0)
    return NULL;

  if (size > MAX_MEMORY)
    return NULL;

  int req_level = 0;
  uint64_t block_size = MAX_MEMORY;
  while (block_size > size && block_size > MIN_BLOCK_SIZE) {
    block_size >>= 1;
    req_level++;
  }

  int index = find_block(mem, 0, 0, req_level);
  if (index == -1)
    return NULL;

  uint64_t offset = (index + 1 - (1 << req_level)) * get_block_size(req_level);
  mem->free_mem -= get_block_size(req_level);

  return (void *)(mem->heap + offset);
}

void buddy_kmm_free(void *ptr, memory_manager_adt m) {
  memory_manager_cdt *mem = (memory_manager_cdt *)m;
  if (mem == NULL || ptr == NULL)
    return;

  if (ptr < (void *)mem->heap || ptr >= (void *)(mem->heap + MAX_MEMORY))
    return;

  uint64_t offset = (uint8_t *)ptr - mem->heap;
  if (offset % MIN_BLOCK_SIZE != 0)
    return;

  int level = 0;
  uint64_t block_size = MAX_MEMORY;
  int index = 0;

  for (; level < LEVELS; level++) {
    int blocks = 1 << level;
    block_size = MAX_MEMORY >> level;

    if (offset % block_size == 0 && offset / block_size < blocks) {
      index = (1 << level) - 1 + (offset / block_size);
      break;
    }
  }

  if (mem->tree[index]) {
    mem->tree[index] = 0;
    mem->free_mem += block_size;
    mark_free(mem, index, level);
  }
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
  // memory_manager_cdt *mem = (memory_manager_cdt *)m;

  // printf("Buddy Allocator Tree (0: free, 1: occupied):\n");
  // int level = 0;
  // for (int i = 0, count = 1; i < TREE_SIZE; level++, count <<= 1) {
  //     printf("Level %d: ", level);
  //     for (int j = 0; j < count && i < TREE_SIZE; j++, i++) {
  //         printf("%d ", mem->tree[i]);
  //     }
  //     printf("\n");
  // }
}

// #endif
