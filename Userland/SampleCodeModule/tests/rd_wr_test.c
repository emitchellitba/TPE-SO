#include <libu.h>
#include <stdLibrary.h>

int rd_wr_test_main(int argc, char *argv[]);

int rd_wr_test_main(int argc, char *argv[]) {
  printf("Starting read/write test...\n");

  char buffer[128];

  int n = read(STDIN, buffer, 10);

  printf("Read from stdin: %s\n", buffer);

  write(STDOUT, "Hello from rd_wr_test!\n", 24);
}
