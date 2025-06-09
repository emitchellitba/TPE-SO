#include <libu.h>
#include <stdLibrary.h>

/**
 * filter - Filtra las vocales del input
 * Programa que recibe una cadena de texto en STDIN y filtra las vocales,
 * dejando solo las consonantes.
 */
int filter_main(int argc, char *argv[]) {
  char buffer[1024];
  int bytes_read;
  int i;

  while ((bytes_read = read(0, buffer, sizeof(buffer) - 1)) > 0) {
    buffer[bytes_read] = '\0';

    for (i = 0; i < bytes_read; i++) {
      char c = buffer[i];
      if (c != 'a' && c != 'e' && c != 'i' && c != 'o' && c != 'u' &&
          c != 'A' && c != 'E' && c != 'I' && c != 'O' && c != 'U') {
        write(1, &c, 1);
      }
    }
  }

  return 0;
}
