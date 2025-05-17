#include <stdint.h>
#include <stdlib.h>

#include "CuTest.h"
#include "dummy_mm.h"
#include "dummy_mm_test.h"

#define MANAGED_MEMORY_SIZE 20480
#define ALLOCATION_SIZE 1024
#define WRITTEN_VALUE 'a'

void testAllocMemory(CuTest *const cuTest);
void testTwoAllocations(CuTest *const cuTest);
void testWriteMemory(CuTest *const cuTest);

static const size_t TestQuantity = 3;
static const Test MemoryManagerTests[] = {testAllocMemory, testTwoAllocations, testWriteMemory};

static MemoryManagerADT memoryManager;

static size_t memoryToAllocate;

static void *allocatedMemory = NULL;
static void *anAllocation = NULL;

CuSuite *getMemoryManagerTestSuite(void) {
	CuSuite *const suite = CuSuiteNew();

	for (size_t i = 0; i < TestQuantity; i++)
		SUITE_ADD_TEST(suite, MemoryManagerTests[i]);

	return suite;
}

inline void givenAMemoryManager(CuTest *const cuTest) {
	void *memoryForMemoryManager = malloc(sizeof(void *));
	if (memoryForMemoryManager == NULL) {
		CuFail(cuTest, "[givenAMemoryManager] Memory for Memory Manager cannot be null");
	}

	void *managedMemory = malloc(MANAGED_MEMORY_SIZE);
	if (managedMemory == NULL) {
		CuFail(cuTest, "[givenAMemoryManager] Managed Memory cannot be null");
	}

	memoryManager = createMemoryManager(memoryForMemoryManager, managedMemory);
}

inline void givenAMemoryAmount() {
	memoryToAllocate = ALLOCATION_SIZE;
}