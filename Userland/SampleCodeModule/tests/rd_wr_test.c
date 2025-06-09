#include <libu.h>
#include <stdLibrary.h>

int rd_wr_test_main(int argc, char *argv[]);

int rd_wr_test_main(int argc, char *argv[]) {
  printf("Starting read/write test...\n");

  printf("Write to stdin and press Enter:\n");

  char buffer[128];

  int n = read(STDIN, buffer, 10);

  printf("Leidos %d caracteres desde STDIN: %s\n", n, buffer);

  printf("Esto esta siendo escrito en STDOUT.\n");

  return 0;
}
