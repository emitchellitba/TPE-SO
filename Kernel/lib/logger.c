#include <logger.h>

#define KMSG_SIZE 8192
struct ringbuf *kmsg = RINGBUF_NEW(KMSG_SIZE);

// TODO: Mover esta funcion a un lugar compartido
static int my_strlen(const char *s) {
  int len = 0;
  while (s && *s++)
    len++;
  return len;
}

static int putc(char c) { return ringbuf_write_overwrite(kmsg, 1, &c); }

static int puts(char *s) {
  s = s ? s : "(null)";

  int len = my_strlen(s);
  ringbuf_write_overwrite(kmsg, len, s);

  return 0;
}

static int putx(uint32_t val) {
  if (!val)
    return puts("0");

  char *enc = "0123456789ABCDEF";
  char buf[9];
  buf[8] = '\0';
  uint8_t i = 8;

  while (val && i) {
    buf[--i] = enc[val & 0xF];
    val >>= 4;
  }

  return puts(&buf[i]);
}

static int putlx(uint64_t val) {
  char *enc = "0123456789ABCDEF";
  char buf[17];
  buf[16] = '\0';
  uint8_t i = 16;
  while (i) {
    buf[--i] = enc[val & 0xF];
    val >>= 4;
  }
  return puts(buf);
}

static int putud(uint32_t val) {
  char buf[11];
  buf[10] = '\0';
  if (!val) {
    buf[9] = '0';
    return puts(&buf[9]);
  }
  uint8_t i = 10;
  while (val) {
    buf[--i] = val % 10 + '0';
    val = (val - val % 10) / 10;
  }
  return puts(buf + i);
}

static int putul(uint64_t val) {
  char buf[21];
  buf[20] = '\0';
  if (!val) {
    buf[19] = '0';
    return puts(&buf[19]);
  }
  uint8_t i = 20;
  while (val) {
    buf[--i] = val % 10 + '0';
    val = (val - val % 10) / 10;
  }
  return puts(buf + i);
}

static int putb(uint8_t val) {
  char buf[9];
  buf[8] = '\0';
  uint8_t i = 8;
  while (i) {
    buf[--i] = '0' + (val & 1);
    val >>= 1;
  }
  return puts(buf);
}

int vprintk(const char *fmt, va_list args) {
  int ret = 0;
  while (*fmt)
    switch (*fmt) {
    case '%':
      ++fmt;
      switch (*fmt) {
      case 'c': /* char */
        ret += putc((char)va_arg(args, int));
        break;
      case 's': /* char * */
        ret += puts((char *)va_arg(args, char *));
        break;
      case 'd': /* decimal */
        ret += putud((uint32_t)va_arg(args, uint32_t));
        break;
      case 'l': /* long */
        switch (*++fmt) {
        case 'x': /* long hex */
          ret += putlx((uint64_t)va_arg(args, uint64_t));
          break;
        case 'd':
          ret += putul((uint64_t)va_arg(args, uint64_t));
          break;
        default:
          ret += putc(*--fmt);
        }
        break;

      case 'b': /* binary */
        ret += putb((uint8_t)(uint32_t)va_arg(args, uint32_t));
        break;
      case 'x': /* Hexadecimal */
        ret += putx((uint32_t)va_arg(args, uint32_t));
        break;
      case 'p': /* Pointer */
        ret += puts("0x");
        ret += putlx((uint64_t)va_arg(args, uint64_t));
        break;
      default:
        ret += putc(*(--fmt));
      }
      ++fmt;
      break;
    default:
      ret += putc(*fmt);
      ++fmt;
    }

  return ret;
}

int printk(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int ret = vprintk(fmt, args);
  va_end(args);

  return ret;
}
