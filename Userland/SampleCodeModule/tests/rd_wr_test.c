#include <libu.h>
#include <stdLibrary.h>

int rd_wr_test_main(int argc, char *argv[]);

int rd_wr_test_main(int argc, char *argv[]) {
  printf("Starting read/write test...\n");

  printf("Write to stdin and press Enter:\n");

  char buffer[128];

  int n = read(STDIN, buffer, 10);

  printf("Read %d from stdin: %s\n", n, buffer);

  write(STDOUT, "Hello from rd_wr_test!\n", 24);

  return 0;
}
