#include <ls.h>

#define MAX_FILE_ENTRIES 16
#define MAX_FILE_NAME_LEN                                                      \
  32 // TODO: esto deberia estar en una libreria compartida

int ls_main(int argc, char *argv[]) {
  char buffer[MAX_FILE_ENTRIES][MAX_FILE_NAME_LEN];
  int n = get_programs(buffer, MAX_FILE_ENTRIES);
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
