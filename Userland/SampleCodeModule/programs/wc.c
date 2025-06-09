// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <libu.h>
#include <stdLibrary.h>

/**
 * wc - word count program
 * Programa que cuenta el número de líneas, palabras y caracteres.
 * Los caracteres los lee de la entrada estándar.
 */
int wc_main(int argc, char *argv[]) {
  char buffer[1024];
  int lines = 0, words = 0, chars = 0;
  int in_word = 0;

  while (1) {
    int bytes_read = read(0, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
      break;
    }
    buffer[bytes_read] = '\0';

    for (int i = 0; i < bytes_read; i++) {
      chars++;
      if (buffer[i] == '\n') {
        lines++;
      }
      if (buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\t') {
        if (in_word) {
          words++;
          in_word = 0;
        }
      } else {
        in_word = 1;
      }
    }
  }

  if (in_word) {
    words++;
  }

  printf("%d %d %d\n", lines, words, chars);
  return 0;
}
