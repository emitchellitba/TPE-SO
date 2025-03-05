#include "stdLibrary.h"
#include <stdarg.h>

#define MAX_BUFF 1000
#define HEXA_MAX 16
#define BUFF_SIZE 500


extern void read(char * buffer, size_t count);
extern void write_stdin(const char * str, size_t count);
extern void get_date_time(const date_time * dt);
extern void screen_clear();
extern void change_color(uint8_t background);
extern void zoom(int in);

void printf(const char * str, ...) {
    va_list args;
    va_start(args, str);
    for (int i=0; str[i] != 0; i++){
        if (str[i] == '%'){
            i++;
            switch (str[i])
            {
            case 'd':
                int num = va_arg(args, int);
                put_int(num);
                break;
            case 'u':
                size_t u_num = va_arg(args, size_t);
                put_uint(u_num);    
                break;
            case 's':
                char * s = va_arg(args, char *);
                put_str(s);
                break;
            case 'c':
                char c = va_arg(args, char);
                put_char(c);
                break;
            case 'x':
                uint64_t hex = va_arg(args, uint64_t);
                put_hex(hex); 
                break;
            default:
                put_char('%');
                break;
            }
        } else if (str[i] == '\\'){
            i++;
            switch (str[i])
            {
            case 'n':
                put_char('\n');
                break;
            case 't':
                put_char('\t');
                break;
            default:
                put_char('\\');
                break;
            }
        } else {
            put_char(str[i]);
        }

    }

    va_end(args);
}

unsigned int create_pseudo_random(unsigned int semilla) {
    // Parámetros del generador congruencial lineal (LCG)
    unsigned int a = 1664525;   // multiplicador
    unsigned int c = 1013904223; // incremento
    unsigned int m = 0xFFFFFFFF; // módulo (2^32 - 1)

    // Generar un nuevo valor pseudoaleatorio
    semilla = (a * semilla + c) & m;

    return semilla;
}

unsigned int get_random() {
    static date_time * time;
    static unsigned int last_value;
    static uint8_t fetched = 0;
    if(!fetched) {
        fetched = 1;
        get_date_time(time);
        last_value = _abs(create_pseudo_random(time->sec));
        return last_value;
    } else {
        last_value = _abs(create_pseudo_random(last_value));
        return last_value;
    }
}

unsigned int _abs(int n) {
    return n < 0 ? -n : n;
}

void put_hex(uint64_t num){
    put_char('0');
    put_char('x');
    uint8_t digit = 0;
    char hexa;
    
    for(int i = HEXA_MAX-2; i >= 0; i--){
		digit = getHexDigit(num, i);
		hexa = hexa_digits[digit];
        put_char(hexa);
	}
}

uint8_t getHexDigit(uint64_t number, int position) {
    number >>= (position * 4); 
    return number & 0xF;
}

void put_str(char * s){ 

    while (*s) {
        put_char(*s++);
    }
}

void put_uint(int n){
    if (n / 10) {
        put_int(n / 10);
    }
    put_char(n % 10 + '0');
}


void put_int(int n){
   if (n < 0) {
        put_char('-');
        n = -n;
    }
    put_uint(n);
}   


void scanf(const char * format, ...) {
    va_list args;
    va_start(args, format);

    char input_buffer[BUFF_SIZE];

    int index = 0, count = 0;

    get_input(input_buffer);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] != '\0') {
            i++; 
            if (format[i] == 'd') {
                int *int_arg = va_arg(args, int *);
                *int_arg = 0;

                while (input_buffer[index] >= '0' && input_buffer[index] <= '9') {
                    *int_arg = (*int_arg * 10) + (input_buffer[index] - '0');
                    index++;
                }
                count++;
            } else if (format[i] == 's') {
                char *str_arg = va_arg(args, char *);
                
                while (input_buffer[index] != ' ' && input_buffer[index] != '\n' && input_buffer[index] != '\0') {
                    *str_arg++ = input_buffer[index++];
                }
                *str_arg = '\0'; 
                count++;
            }
        } else if (format[i] == input_buffer[index]) {
            index++; 
        } else {
            break; 
        }
    }

}

char get_entry() {
    char buffer[1];
    buffer[0] = '\0';
    read(buffer, 1);
    return buffer[0];
}

char get_char() {
    char c = get_entry();
    while(c == '\0') {
        c = get_entry();
    }
    return c;
}

void put_char(const char c) {
    write_stdin(&c, 1);
    return;
}

void load_date_time(date_time * dt){
    get_date_time(dt);
}

void clear(){
    screen_clear();
}

void zoom_in() {
    zoom(1);
}

void zoom_out() {
    zoom(0);
}

int str_cmp(const char* str1, const char* str2) {
    while(*str1 != '\0' && *str2 != '\0') {
        if(*str1 != *str2)
            return *str1 - *str2;
        str1++;
        str2++;
    }
    if(*str1 == '\0' || *str2 == '\0')
        return *str1 - *str2;
}

int str_ncmp(const char* str1, const char* str2, int n) {
    int i = 0;
    while(*str1 != '\0' && *str2 != '\0' && i < n) {
        if(*str1 != *str2)
            return *str1 - *str2;
        str1++;
        str2++;
    }
    if(*str1 == '\0' || *str2 == '\0')
        return *str1 - *str2;
}

int str_len(const char* str) {
    int len = 0;
    while(*str++ != '\0')
        len++;
    return len; 
}

void to_lower(char * str){
    for (int i=0; str[i] != 0; i++){
        if ('A' <= str[i] && str[i] <= 'Z'){
            str[i] += ('a'-'A');
        }
    }
}

void change_font_color() {
    change_color(0);
}

void change_bg_color() {
    change_color(1);
}


