#include "heap.h"
#include "../include/lib/memory_manager.h"
#include <naiveConsole.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint8_t *heap = (uint8_t *)HEAP_BASE;

typedef struct block_header_t {
  size_t size;
  int is_free;
  struct block_header_t *next;
} block_header_t;

#define ALIGN4(x) (((x) + 3) & ~3)
#define HEADER_SIZE (sizeof(block_header_t))

static block_header_t *free_list = NULL;

void init_heap() {
  free_list = (block_header_t *)heap;
  free_list->size = HEAP_SIZE;
  free_list->is_free = 1;
  free_list->next = NULL;
}

void split_block(block_header_t *block, size_t size) {
  block_header_t *new_block = (block_header_t *)((uint8_t *)block + size);
  new_block->size = block->size - size;
  new_block->is_free = 1;
  new_block->next = block->next;

  block->size = size;
  block->next = new_block;
}

void *my_malloc(size_t size) {
  if (size == 0)
    return NULL;
  size = ALIGN4(size + HEADER_SIZE);
  block_header_t *current = free_list;

  while (current != NULL) {
    if (current->is_free && current->size >= size) {
      if (current->size >= size + HEADER_SIZE + 4) {
        split_block(current, size);
      }
      current->is_free = 0;
      return (void *)((uint8_t *)current + HEADER_SIZE);
    }
    current = current->next;
  }
  return NULL;
}

void coalesce() {
  block_header_t *current = free_list;
  while (current && current->next) {
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

  block_header_t *block = (block_header_t *)((uint8_t *)ptr - HEADER_SIZE);
  block->is_free = 1;
  coalesce();
}

void print_memory_state() {
  block_header_t *current = (block_header_t *)heap;
  int block_num = 0;
  int free_blocks = 0;
  int used_blocks = 0;

  ncPrint("\n======== ESTADO DEL HEAP ========\n");

  while ((uint8_t *)current < heap + HEAP_SIZE) {
    ncPrint("Bloque #");
    ncPrintDec(block_num++);
    ncPrint(" en ");
    ncPrintHex((uint64_t)current);
    ncPrint(" | Estado: ");
    if (current->is_free) {
      ncPrint("LIBRE ✅");
      free_blocks++;
    } else {
      ncPrint("OCUPADO ❌");
      used_blocks++;
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
  ncPrint("\n==================================\n");
}
