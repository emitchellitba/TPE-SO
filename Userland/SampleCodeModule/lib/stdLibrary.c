#include <libu.h>
#include <stdLibrary.h>
#include <stdarg.h>
#include <stdbool.h>

#define MAX_BUFF 1000
#define HEXA_MAX 16
#define BUFF_SIZE 500

extern void get_date_time(const date_time *dt);

char hexa_digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

int itoa(int value, char *buffer) {
  char tmp[20];
  int i = 0, neg = 0;

  if (value == 0) {
    buffer[0] = '0';
    buffer[1] = '\0';
    return 1;
  }

  if (value < 0) {
    neg = 1;
    value = -value;
  }

  while (value > 0) {
    tmp[i++] = (value % 10) + '0';
    value /= 10;
  }

  int len = 0;
  if (neg)
    buffer[len++] = '-';

  while (i > 0)
    buffer[len++] = tmp[--i];
  buffer[len] = '\0';
  return len;
}

void itos(int value, char *buffer) {
    int i = 0, neg = 0;
    char temp[12]; // Enough for 32-bit int

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    if (value < 0) {
        neg = 1;
        value = -value;
    }

    while (value > 0) {
        temp[i++] = (value % 10) + '0';
        value /= 10;
    }

    if (neg)
        temp[i++] = '-';

    // Reverse the string into buffer
    int j = 0;
    while (i > 0)
        buffer[j++] = temp[--i];
    buffer[j] = '\0';
}

int utoa(size_t value, char *buffer) {
  char tmp[20];
  int i = 0;

  if (value == 0) {
    buffer[0] = '0';
    buffer[1] = '\0';
    return 1;
  }

  while (value > 0) {
    tmp[i++] = (value % 10) + '0';
    value /= 10;
  }

  int len = 0;
  while (i > 0)
    buffer[len++] = tmp[--i];
  buffer[len] = '\0';
  return len;
}

int xtoa(uint64_t value, char *buffer) {
  char tmp[20];
  int i = 0;
  const char *digits = "0123456789abcdef";

  if (value == 0) {
    buffer[0] = '0';
    buffer[1] = '\0';
    return 1;
  }

  while (value > 0) {
    tmp[i++] = digits[value % 16];
    value /= 16;
  }

  int len = 0;
  while (i > 0)
    buffer[len++] = tmp[--i];
  buffer[len] = '\0';
  return len;
}

void printf(const char *str, ...) {
  va_list args;
  va_start(args, str);
  for (int i = 0; str[i] != 0; i++) {
    if (str[i] == '%') {
      i++;
      bool left_align = false;
      int width = 0;

      // flags
      if (str[i] == '-') {
        left_align = true;
        i++;
      }

      // width
      while (str[i] >= '0' && str[i] <= '9') {
        width = width * 10 + (str[i] - '0');
        i++;
      }

      char buffer[64];
      int len = 0;

      switch (str[i]) {
      case 'd': {
        int num = va_arg(args, int);
        len = itoa(num, buffer);
        break;
      }
      case 'u': {
        size_t num = va_arg(args, size_t);
        len = utoa(num, buffer);
        break;
      }
      case 'x': {
        uint64_t hex = va_arg(args, uint64_t);
        len = xtoa(hex, buffer);
        break;
      }
      case 's': {
        char *s = va_arg(args, char *);
        int s_len = 0;
        while (s[s_len])
          s_len++;

        if (width > s_len && !left_align)
          for (int j = 0; j < width - s_len; j++)
            put_char(' ');
        put_str(s);
        if (width > s_len && left_align)
          for (int j = 0; j < width - s_len; j++)
            put_char(' ');
        continue;
      }
      case 'c': {
        char c = (char)va_arg(args, int);
        put_char(c);
        continue;
      }
      default:
        put_char('%');
        put_char(str[i]);
        continue;
      }

      if (width > len && !left_align)
        for (int j = 0; j < width - len; j++)
          put_char(' ');
      put_str(buffer);
      if (width > len && left_align)
        for (int j = 0; j < width - len; j++)
          put_char(' ');
    } else if (str[i] == '\\') {
      i++;
      switch (str[i]) {
      case 'n':
        put_char('\n');
        break;
      case 't':
        put_char('\t');
        break;
      default:
        put_char('\\');
        put_char(str[i]);
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
  unsigned int a = 1664525;    // multiplicador
  unsigned int c = 1013904223; // incremento
  unsigned int m = 0xFFFFFFFF; // módulo (2^32 - 1)

  // Generar un nuevo valor pseudoaleatorio
  semilla = (a * semilla + c) & m;

  return semilla;
}

unsigned int get_random() {
  static date_time *time;
  static unsigned int last_value;
  static uint8_t fetched = 0;
  if (!fetched) {
    fetched = 1;
    get_date_time(time);
    last_value = _abs(create_pseudo_random(time->sec));
    return last_value;
  } else {
    last_value = _abs(create_pseudo_random(last_value));
    return last_value;
  }
}

unsigned int _abs(int n) { return n < 0 ? -n : n; }

void put_hex(uint64_t num) {
  put_char('0');
  put_char('x');
  uint8_t digit = 0;
  char hexa;

  for (int i = HEXA_MAX - 2; i >= 0; i--) {
    digit = getHexDigit(num, i);
    hexa = hexa_digits[digit];
    put_char(hexa);
  }
}

uint8_t getHexDigit(uint64_t number, int position) {
  number >>= (position * 4);
  return number & 0xF;
}

void put_str(char *s) {

  while (*s) {
    put_char(*s++);
  }
}

void put_uint(int n) {
  if (n / 10) {
    put_int(n / 10);
  }
  put_char(n % 10 + '0');
}

void put_int(int n) {
  if (n < 0) {
    put_char('-');
    n = -n;
  }
  put_uint(n);
}

char get_entry() {
  char buffer[1];
  buffer[0] = '\0';
  read(0, buffer, 1);
  return buffer[0];
}

char get_char() {
  char c;
  int n = read(STDIN, &c, 1);
  return n <= 0 ? 0 : c;
}

void put_char(const char c) {
  write(1, &c, 1);
  return;
}

void load_date_time(date_time *dt) { get_date_time(dt); }

int str_cmp(const char *str1, const char *str2) {
  while (*str1 != '\0' && *str2 != '\0') {
    if (*str1 != *str2)
      return *str1 - *str2;
    str1++;
    str2++;
  }
  if (*str1 == '\0' || *str2 == '\0')
    return *str1 - *str2;
  return 0;
}

int str_ncmp(const char *str1, const char *str2, int n) {
  int i = 0;
  while (*str1 != '\0' && *str2 != '\0' && i < n) {
    if (*str1 != *str2)
      return *str1 - *str2;
    str1++;
    str2++;
    i++;
  }
  if (*str1 == '\0' || *str2 == '\0')
    return *str1 - *str2;
  return 0;
}

int str_len(const char *str) {
  int len = 0;
  while (*str++ != '\0')
    len++;
  return len;
}

char *str_chr(const char *str, int c) {
  while (*str) {
    if (*str == (char)c)
      return (char *)str;
    str++;
  }
  if (c == '\0')
    return (char *)str;
  return 0;
}

char *str_tok(char *str, const char *delim) {
  static char *last = 0;
  if (str)
    last = str;
  if (!last)
    return 0;

  char *start = last;
  while (*start && str_chr(delim, *start))
    start++;

  if (!*start) {
    last = 0;
    return 0;
  }

  char *end = start;
  while (*end && !str_chr(delim, *end))
    end++;

  if (*end) {
    *end = '\0';
    last = end + 1;
  } else {
    last = 0;
  }

  return start;
}

void to_lower(char *str) {
  for (int i = 0; str[i] != 0; i++) {
    if ('A' <= str[i] && str[i] <= 'Z') {
      str[i] += ('a' - 'A');
    }
  }
}

char *trim(char *str) {
  while (*str == ' ' || *str == '\t' || *str == '\n')
    str++;

  char *end = str + str_len(str) - 1;

  while (end > str && (*end == ' ' || *end == '\t' || *end == '\n'))
    end--;

  *(end + 1) = '\0';

  return str;
}

void str_ncpy(char *dest, const char *src, int n) {
  int i;
  for (i = 0; i < n && src[i] != '\0'; i++) {
    dest[i] = src[i];
  }
  dest[i] = '\0';
}

int atoi(const char *str) {
  int res = 0;
  int sign = 1;

  while (*str == ' ' || *str == '\t' || *str == '\n')
    str++;

  if (*str == '-') {
    sign = -1;
    str++;
  } else if (*str == '+') {
    str++;
  }

  while (*str >= '0' && *str <= '9') {
    res = res * 10 + (*str - '0');
    str++;
  }

  return sign * res;
}
