// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <libu.h>
#include <stdLibrary.h>
#include <sys/types.h>

#define TIME_INTERVAL 3 // Intervalo de tiempo en segundos entre cada iteración

int loop_main(int argc, char *argv[]) {
  pid_t pid = get_pid();

  while (1) {
    printf("Process %d says: Keep calm and loop on!\n", pid);
    sleep_time(TIME_INTERVAL);
  }
  return 0;
}
