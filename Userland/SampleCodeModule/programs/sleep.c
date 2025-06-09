// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <libu.h>
#include <stdLibrary.h>

int sleep_main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: sleep <seconds>\n");
    return -1;
  }

  int seconds = atoi(argv[1]);
  if (seconds <= 0) {
    printf("Error: sleep requires a positive number of seconds to wait.\n");
    return -1;
  }

  printf("Sleeping for %d seconds...\n", seconds);
  sleep_time(seconds);
  printf("Woke up after %d seconds.\n", seconds);

  return 0;
}
