#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <videoDriver.h>
#include "../include/lib/memory_manager.h"

#define BLOCK_SIZE 128
#define TOTAL_BLOCKS (HEAP_SIZE / BLOCK_SIZE)

// Metadata for each block in the heap
typedef struct {
    int is_free;        // 1 = free, 0 = in use
    size_t block_count; // Only first block of each assignation holds the count
} BlockInfo;

typedef struct {
    BlockInfo *block_info;
    void* memory;
} memory_manager_cdt;

static BlockInfo block_info[TOTAL_BLOCKS];
static memory_manager_cdt kmm = {0};

memory_manager_adt my_kmm_init(void *memory_to_manage) {
    kmm.memory = memory_to_manage;
    kmm.block_info = block_info;
    for (size_t i = 0; i < TOTAL_BLOCKS; i++) {
        kmm.block_info[i].is_free = 1;
        kmm.block_info[i].block_count = 0;
    }
    return (memory_manager_adt)kmm.memory;
}

void* my_kmalloc(memory_manager_adt restrict mm, size_t size) {
    size_t blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int i = 0; i <= TOTAL_BLOCKS - blocks_needed; i++) {
        int found = 1;
        for (int j = 0; j < blocks_needed; j++) {
            if (!kmm.block_info[i + j].is_free) {
                found = 0;
                i += j;
                break;
            }
        }
        if (found) {
            for (int j = 0; j < blocks_needed; j++) {
                kmm.block_info[i + j].is_free = 0;
                if(j == 0) {
                    kmm.block_info[i + j].block_count = blocks_needed; // Only first block
                } else {
                    kmm.block_info[i + j].block_count = 0; // Concurrent blocks marked as used, but not the first
                }
            }
            return kmm.memory + i * BLOCK_SIZE;
        }
    }
    return NULL;
}

void my_kmm_free(memory_manager_adt mm, void *ptr){
    if (!ptr || ptr < kmm.memory || ptr >= kmm.memory + HEAP_SIZE) return;

    size_t index = (ptr - kmm.memory) / BLOCK_SIZE;
    size_t blocks_to_free = kmm.block_info[index].block_count;

    for (size_t i = 0; i < blocks_to_free; i++) {
        kmm.block_info[index + i].is_free = 1;
        kmm.block_info[index + i].block_count = 0;
    }
}

void my_kmm_dump_state(memory_manager_adt mem) {
    size_t free_blocks = 0, used_blocks = 0, free_regions = 0, used_regions = 0;

    int in_free = 0, in_used = 0;
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (kmm.block_info[i].is_free) {
            free_blocks++;
            if (!in_free) {
                free_regions++;
                in_free = 1;
                in_used = 0;
            }
        } else {
            used_blocks++;
            if (!in_used) {
                used_regions++;
                in_used = 1;
                in_free = 0;
            }
        }
    }

    char buffer[20];
    print_str("=== Heap State ===\n", 20, 0);
    print_str("Block size:", 12, 0);
    utoa(BLOCK_SIZE, buffer, 10);
    print_str(buffer, str_len(buffer), 0);
    print_str("bytes\n", 7, 0);
    print_str("Total blocks: ", 15, 0);
    utoa(TOTAL_BLOCKS, buffer, 10);
    print_str(buffer, str_len(buffer), 0);
    print_str("\n", 1, 0);
    print_str("Used blocks: ", 14, 0);
    utoa(used_blocks, buffer, 10);
    print_str(buffer, str_len(buffer), 0);
    print_str("\n", 1, 0);
    print_str("Free blocks: ", 14, 0);
    utoa(free_blocks, buffer, 10);
    print_str(buffer, str_len(buffer), 0);
    print_str("\n", 1, 0);
    print_str("Used regions: ", 15, 0);
    utoa(used_regions, buffer, 10);
    print_str(buffer, str_len(buffer), 0);
    print_str("\n", 1, 0);
    print_str("Free regions: ", 15, 0);
    utoa(free_regions, buffer, 10);
    print_str(buffer, str_len(buffer), 0);
    print_str("\n", 1, 0);

    // Optional: print a visual map
    print_str("Heap map: ", 11, 0);
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        print(kmm.block_info[i].is_free ? (const char*)'.' : (const char*)'#', 1, 0);
    }
    print_str("\n", 1, 0);
}
