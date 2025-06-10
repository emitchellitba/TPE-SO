
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include "test_util.h"
#include <libu.h>
#include <stdLibrary.h>

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

int test_mm(uint64_t argc, char *argv[]) {
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory = 10000;
  int j = 0;

  if (argc != 1) {
    printf("Incorrect argument count. Expected: 1\n");
    return -1;
  }

  while (1) {
    rq = 0;
    total = 0;
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = (void *)m_malloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        my_memset(mm_rqs[i].address, i, mm_rqs[i].size);
      }
    }

    // Check
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("Memory Error.");
          return -1;
        }
      }
    }

    // Free
    for (i = 0; i < rq; i++) {
      if (mm_rqs[i].address) {
        m_free(mm_rqs[i].address);
      }
    }

    j++;

    if (j >= 3) {
      break;
    }
  }
  printf("Memory test completed successfully.\n");

  return 0;
}
