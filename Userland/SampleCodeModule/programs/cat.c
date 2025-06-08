#include <libu.h>
#include <sys/types.h>

/* Este programa lee de STDIN e imprime lo que leyo */
int cat_main(int argc, char *argv[]) {
  char buffer[1024];
  ssize_t bytes_read;
  while ((bytes_read = read(STDIN, buffer, sizeof(buffer) - 1)) > 0) {
    buffer[bytes_read] = '\0';
    write(STDOUT, buffer, bytes_read);
  }

  if (bytes_read < 0) {
    write(STDERR, "Error reading from STDIN\n", 25);
    return 1;
  }

  return 0;
}
