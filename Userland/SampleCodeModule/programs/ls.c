#include <libu.h>
#include <stdLibrary.h>

#define MAX_FILE_ENTRIES 16
#define MAX_FILE_NAME_LEN 32

int ls_main(int argc, char *argv[]) {
  char buffer[MAX_FILE_ENTRIES][MAX_FILE_NAME_LEN];
  int n = get_programs((char **)buffer, MAX_FILE_ENTRIES);
  if (n < 0) {
    printf("Error listing programs\n");
    return -1;
  }

  printf("Available programs:\n");
  for (int i = 0; i < n; i++) {
    printf("\t- %s\n", buffer[i]);
  }

  return 0;
}
