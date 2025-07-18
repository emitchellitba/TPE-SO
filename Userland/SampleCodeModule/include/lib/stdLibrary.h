#ifndef _STDLIBRARY_H_
#define _STDLIBRARY_H_

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint8_t year;
} date_time;

// TODO: Esta funcion viene de shell.c, una libreria no deberia
// depender de un modulo. Aparte de que hay dependencia circular.

void printf(const char *str, ...);
// void scanf(const char *formatt, ...);
char get_char();
char get_entry();
void put_char(const char c);
void load_date_time(date_time *dt);
unsigned int get_random();
void put_hex(uint64_t num);
uint8_t getHexDigit(uint64_t number, int position);
void put_str(char *s);
void put_uint(int n);
void put_int(int n);
int str_cmp(const char *str1, const char *str2);
int str_ncmp(const char *str1, const char *str2, int n);
int str_len(const char *str);
char *str_tok(char *str, const char *delim);
char *str_chr(const char *str, int c);
void str_ncpy(char *dest, const char *src, int n);
void to_lower(char *str);
unsigned int _abs(int a);
void itos(int value, char *buffer);
char *trim(char *str);
int atoi(const char *str);

#endif // _STDLIBRARY_H_
