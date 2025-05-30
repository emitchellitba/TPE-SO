#ifndef LIB_H
#define LIB_H

#include <stdint.h>

typedef unsigned long size_t;

typedef enum { STDIN = 0, STDOUT, STDERR } FDS;

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);
char *cpu_vendor(char *result);
int str_len(const char *str);
char *str_cpy(char *dest, const char *src);
extern void main_ret();
int abs(int x);

uint8_t inb(uint8_t a);
void outb(uint8_t a, uint8_t b);

#endif
