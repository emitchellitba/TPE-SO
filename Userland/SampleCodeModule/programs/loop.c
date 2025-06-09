#include <libu.h>
#include <stdlib.h>
#include <sys/types.h>

#define TIME_INTERVAL 3 // Intervalo de tiempo en segundos entre cada iteración

int loop_main(int argc, char *argv[]) {
    pid_t pid = get_pid();

    while(1) {
        printf("Process %d says: Keep calm and loop on!\n", pid);
        sleep_time(TIME_INTERVAL);
    }
    return 0;
}