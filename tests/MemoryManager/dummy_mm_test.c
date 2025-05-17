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
void testExhaustion(CuTest *const cuTest);
void testZeroAllocation(CuTest *const cuTest);

static const size_t TestQuantity = 4;
static const Test MemoryManagerTests[] = {testAllocMemory, testTwoAllocations, testWriteMemory, testZeroAllocation};

static inline void givenAMemoryManager(CuTest *const cuTest);
static inline void givenAMemoryAmount(void);
static inline void givenAnAllocation(void);
static inline void whenMemoryIsAllocated(void);
static inline void whenMemoryIsWritten(void);
static inline void thenSomeMemoryIsReturned(CuTest *const cuTest);
static inline void thenTheTwoAdressesAreDifferent(CuTest *const cuTest);
static inline void thenBothDoNotOverlap(CuTest *const cuTest);
static inline void thenMemorySuccessfullyWritten(CuTest *const cuTest);

static inline void whenZeroBytesAreRequested(void);
static inline void thenNullIsReturned(CuTest *const cuTest);

static MemoryManagerADT memoryManager;

static size_t memoryToAllocate;

static void *allocatedMemory = NULL;
static void *anAllocation = NULL;

CuSuite *getMemoryManagerTestSuite(void)
{
	CuSuite *const suite = CuSuiteNew();

	for (size_t i = 0; i < TestQuantity; i++)
		SUITE_ADD_TEST(suite, MemoryManagerTests[i]);

	return suite;
}

void testAllocMemory(CuTest *const cuTest)
{
	givenAMemoryManager(cuTest);
	givenAMemoryAmount();

	whenMemoryIsAllocated();

	thenSomeMemoryIsReturned(cuTest);
}

void testTwoAllocations(CuTest *const cuTest)
{
	givenAMemoryManager(cuTest);
	givenAMemoryAmount();
	givenAnAllocation();

	whenMemoryIsAllocated();

	thenSomeMemoryIsReturned(cuTest);
	thenTheTwoAdressesAreDifferent(cuTest);
	thenBothDoNotOverlap(cuTest);
}

void testWriteMemory(CuTest *const cuTest)
{
	givenAMemoryManager(cuTest);
	givenAMemoryAmount();
	givenAnAllocation();

	whenMemoryIsWritten();

	thenMemorySuccessfullyWritten(cuTest);
}

void testZeroAllocation(CuTest *const cuTest)
{
	givenAMemoryManager(cuTest);
	whenZeroBytesAreRequested();
	thenNullIsReturned(cuTest);
}

inline void givenAMemoryManager(CuTest *const cuTest)
{
	void *managedMemory = malloc(MANAGED_MEMORY_SIZE);
	if (managedMemory == NULL)
	{
		CuFail(cuTest, "[givenAMemoryManager] Managed Memory cannot be null");
	}

	memoryManager = kmm_init(managedMemory);
}

inline void givenAMemoryAmount()
{
	memoryToAllocate = ALLOCATION_SIZE;
}

inline void givenAnAllocation()
{
	anAllocation = kmalloc(memoryManager, memoryToAllocate);
}

inline void whenMemoryIsAllocated()
{
	allocatedMemory = kmalloc(memoryManager, memoryToAllocate);
}

inline void whenMemoryIsWritten()
{
	*((char *)anAllocation) = WRITTEN_VALUE;
}

inline void thenSomeMemoryIsReturned(CuTest *const cuTest)
{
	CuAssertPtrNotNull(cuTest, allocatedMemory);
}

inline void thenTheTwoAdressesAreDifferent(CuTest *const cuTest)
{
	CuAssertTrue(cuTest, anAllocation != allocatedMemory);
}

inline void thenBothDoNotOverlap(CuTest *const cuTest)
{
	int distance = (char *)anAllocation - (char *)allocatedMemory;
	distance = abs(distance);

	CuAssertTrue(cuTest, distance >= ALLOCATION_SIZE);
}

inline void thenMemorySuccessfullyWritten(CuTest *const cuTest)
{
	uint8_t writtenValue = WRITTEN_VALUE;
	uint8_t readValue = *((uint8_t *)anAllocation);

	CuAssertIntEquals(cuTest, writtenValue, readValue);
}

static inline void whenZeroBytesAreRequested()
{
	allocatedMemory = kmalloc(memoryManager, 0);
}

static inline void thenNullIsReturned(CuTest *const cuTest)
{
	CuAssertPtrEquals(cuTest, NULL, allocatedMemory);
}
