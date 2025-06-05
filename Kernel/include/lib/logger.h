#ifndef LOGGER_H
#define LOGGER_H

#include "../ds/ringbuf.h"

#include <stdarg.h>
#include <stdint.h>

#define LOG_NONE 0    // No logging (No definir log functions con este nivel)
#define LOG_EMERG 1   // System is unusable
#define LOG_ALERT 2   // Action must be taken immediately
#define LOG_CRIT 3    // Critical conditions
#define LOG_ERR 4     // Error conditions
#define LOG_WARNING 5 // Warning conditions
#define LOG_NOTICE 6  // Normal but significant condition
#define LOG_INFO 7    // Informational
#define LOG_DEBUG 8   // Debug-level messages

int vprintk(const char *fmt, va_list args);
int printk(const char *fmt, ...);

#define LOGGER_DEFINE(module, name, _level)                                    \
  int name(int level, const char *fmt, ...) {                                  \
    if (level <= _level) {                                                     \
      va_list args;                                                            \
      va_start(args, fmt);                                                     \
      printk("%s: ", #module);                                                 \
      vprintk(fmt, args);                                                      \
      va_end(args);                                                            \
    }                                                                          \
    return 0;                                                                  \
  }

#define LOGGER_DECLARE(name) int name(int level, const char *fmt, ...);

#endif // LOGGER_H
