#ifndef TESTS_H
#define TESTS_H

typedef struct {
  const char *display_name;
  const char *program_name;
  const char *description;
} test_entry_t;

void print_available_tests();
int test_runner_cmd(int argc, char **argv);

#endif // TESTS_H
