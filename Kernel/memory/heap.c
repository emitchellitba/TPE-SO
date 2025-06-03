#include "../include/lib/memory_manager.h"
#include "../include/naiveConsole.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_HEAP_BASE 0x1000000 // 16MB
#define HEAP_HEAP_SIZE 0x1000000 // 16MB

static uint8_t *heap = (uint8_t *)HEAP_HEAP_BASE;
static uint8_t *heap_end = (uint8_t *)(HEAP_HEAP_BASE + HEAP_HEAP_SIZE);

typedef struct block_header_t {
  size_t size;
  int is_free;
  struct block_header_t *next;
  uint32_t magic; // Add magic number for validation
} block_header_t;

#define ALIGN4(x) (((x) + 3) & ~3)
#define HEADER_SIZE (sizeof(block_header_t))
#define MAGIC_NUMBER 0xDEADBEEF
#define MIN_BLOCK_SIZE (HEADER_SIZE + 4) // Minimum size for a block

static block_header_t *free_list = NULL;
static size_t total_allocated = 0; // Track total allocated memory

// Helper function to validate a block
static int is_valid_block(block_header_t *block) {
  return block != NULL && block->magic == MAGIC_NUMBER &&
         (uint8_t *)block >= heap && (uint8_t *)block < heap_end;
}

void init_heap() {
  free_list = (block_header_t *)heap;
  free_list->size = HEAP_HEAP_SIZE;
  free_list->is_free = 1;
  free_list->next = NULL;
  free_list->magic = MAGIC_NUMBER;
  total_allocated = 0;
}

void split_block(block_header_t *block, size_t size) {
  if (!is_valid_block(block))
    return;

  size_t remaining_size = block->size - size;
  if (remaining_size < MIN_BLOCK_SIZE)
    return; // Don't split if too small

  block_header_t *new_block = (block_header_t *)((uint8_t *)block + size);
  new_block->size = remaining_size;
  new_block->is_free = 1;
  new_block->next = block->next;
  new_block->magic = MAGIC_NUMBER;

  block->size = size;
  block->next = new_block;
}

void *my_malloc(size_t size) {
  if (size == 0)
    return NULL;

  // Add header size and align
  size = ALIGN4(size + HEADER_SIZE);
  if (size < MIN_BLOCK_SIZE)
    size = MIN_BLOCK_SIZE;

  // Check if we have enough total memory
  if (total_allocated + size > HEAP_HEAP_SIZE) {
    return NULL;
  }

  block_header_t *current = free_list;
  block_header_t *best_fit = NULL;
  size_t min_size = HEAP_HEAP_SIZE + 1; // Start with a size larger than heap

  // First fit with best fit strategy
  while (current != NULL) {
    if (current->is_free && current->size >= size) {
      if (current->size < min_size) {
        best_fit = current;
        min_size = current->size;
      }
    }
    current = current->next;
  }

  if (best_fit == NULL)
    return NULL;

  // Split if enough space
  if (best_fit->size >= size + MIN_BLOCK_SIZE) {
    split_block(best_fit, size);
  }

  best_fit->is_free = 0;
  total_allocated += best_fit->size;
  return (void *)((uint8_t *)best_fit + HEADER_SIZE);
}

void coalesce() {
  block_header_t *current = free_list;
  while (current && current->next) {
    if (!is_valid_block(current) || !is_valid_block(current->next)) {
      break; // Invalid block detected
    }

    if (current->is_free && current->next->is_free) {
      current->size += current->next->size;
      current->next = current->next->next;
    } else {
      current = current->next;
    }
  }
}

void my_free(void *ptr) {
  if (!ptr)
    return;

  // Validate pointer is within heap bounds
  if (ptr < (void *)heap || ptr >= (void *)heap_end) {
    return; // Invalid pointer
  }

  block_header_t *block = (block_header_t *)((uint8_t *)ptr - HEADER_SIZE);

  // Validate block
  if (!is_valid_block(block)) {
    return; // Invalid block
  }

  // Check for double free
  if (block->is_free) {
    return; // Double free detected
  }

  block->is_free = 1;
  total_allocated -= block->size;
  coalesce();
}

void print_memory_state() {
  block_header_t *current = (block_header_t *)heap;
  int block_num = 0;
  int free_blocks = 0;
  int used_blocks = 0;
  size_t total_free = 0;
  size_t total_used = 0;

  ncPrint("\n======== ESTADO DEL HEAP ========\n");

  while ((uint8_t *)current < heap_end) {
    if (!is_valid_block(current)) {
      ncPrint("ERROR: Bloque inválido detectado!\n");
      break;
    }

    ncPrint("Bloque #");
    ncPrintDec(block_num++);
    ncPrint(" en ");
    ncPrintHex((uint64_t)current);
    ncPrint(" | Estado: ");
    if (current->is_free) {
      ncPrint("LIBRE ✅");
      free_blocks++;
      total_free += current->size - HEADER_SIZE;
    } else {
      ncPrint("OCUPADO ❌");
      used_blocks++;
      total_used += current->size - HEADER_SIZE;
    }
    ncPrint(" | Tamaño total: ");
    ncPrintDec(current->size);
    ncPrint(" bytes | Útil: ");
    ncPrintDec(current->size > HEADER_SIZE ? current->size - HEADER_SIZE : 0);
    ncPrint(" bytes\n");

    if (!current->next)
      break;
    current = current->next;
  }

  ncPrint("==================================\n");
  ncPrint("Bloques ocupados: ");
  ncPrintDec(used_blocks);
  ncPrint(" | Libres: ");
  ncPrintDec(free_blocks);
  ncPrint(" | Total: ");
  ncPrintDec(free_blocks + used_blocks);
  ncPrint("\n");
  ncPrint("Memoria ocupada: ");
  ncPrintDec(total_used);
  ncPrint(" bytes | Libre: ");
  ncPrintDec(total_free);
  ncPrint(" bytes\n");
  ncPrint("==================================\n");
}
