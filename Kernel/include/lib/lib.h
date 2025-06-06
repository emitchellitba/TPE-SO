#ifndef LIB_H
#define LIB_H

#include <stdint.h>

typedef unsigned long size_t;

typedef enum { STDIN = 0, STDOUT, STDERR } FDS;

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);
void *memmove(void *dest, const void *src, uint64_t n);
char *cpu_vendor(char *result);
int str_len(const char *str);
char *str_cpy(char *dest, const char *src);
int str_cmp(const char *s1, const char *s2);
char *str_ncpy(char *dest, const char *src, size_t n);

int abs(int x);

uint8_t inb(uint8_t a);
void outb(uint8_t a, uint8_t b);

#endif
