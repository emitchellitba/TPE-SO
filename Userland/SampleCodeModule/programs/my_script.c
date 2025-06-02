#include <my_script.h>

int my_script_main(int argc, char *argv[]) {
  printf("Hello, World!\n");

  if (argc > 1) {
    printf("Arguments passed to the script:\n");
    for (int i = 1; i < argc; i++) {
      printf("Argument %d: %s\n", i, argv[i]);
    }
  } else {
    printf("No arguments passed to the script.\n");
  }

  proc_exit(0);
}
