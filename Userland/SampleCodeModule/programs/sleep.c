#include <libu.h>
#include <stdLibrary.h>

int sleep_main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: sleep <ticks>\n");
    return -1;
  }

  int ticksToWait = atoi(argv[1]);
  if (ticksToWait <= 0) {
    printf("Error: sleep requires a positive number of ticks to wait.\n");
    return -1;
  }

  printf("Sleeping for %d ticks...\n", ticksToWait);
  sleep_time(ticksToWait);
  printf("Woke up after %d ticks.\n", ticksToWait);

  return 0;
}
