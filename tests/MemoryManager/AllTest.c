
#include <stdio.h>

#include "CuTest.h"
#include "dummy_mm.h"
#include "dummy_mm_test.h"

void RunAllTests(void) {
	CuString *output = CuStringNew();
	CuSuite *suite = CuSuiteNew();

	CuSuiteAddSuite(suite, getMemoryManagerTestSuite());

	CuSuiteRun(suite);

	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);

	printf("%s\n", output->buffer);
}

int main(void) {
	RunAllTests();
	return 0;
}
