#ifndef _STDLIBRARY_H_
#define _STDLIBRARY_H_
#include <stddef.h>
#include <stdint.h>


typedef struct{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
}date_time;

static char hexa_digits[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void printf(const char * str, ...);
void scanf(const char * formatt, ...);
char get_char();
char get_entry();
void put_char(const char c);
void load_date_time(date_time *dt);
unsigned int get_random();
void put_hex(uint64_t num);
uint8_t getHexDigit(uint64_t number, int position);
void put_str(char * s);
void put_uint(int n);
void put_int(int n);
void clear();   
void zoom_in();
void zoom_out();
int str_cmp(const char* str1, const char* str2);
int str_ncmp(const char* str1, const char* str2, int n);
int str_len(const char* str);
char* str_tok(char * str1, const char* str2);
void to_lower(char * str);
void change_font_color();
void change_bg_color();
unsigned int _abs(int a);
#endif
