#include <stdint.h>

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
void *my_memset(void *destination, int32_t c, uint64_t length);
int64_t satoi(char *str);
void endless_loop();
void endless_loop_print(int argc, char *argv[]);
