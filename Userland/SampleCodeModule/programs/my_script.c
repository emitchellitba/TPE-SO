// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <libu.h>
#include <stdLibrary.h>

int my_script_main(int argc, char *argv[]);

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

  return 0;
}
