#ifndef _TIME_H_
#define _TIME_H_
#include <stdint.h>

typedef struct {
  uint8_t sec;
  uint8_t min;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint8_t year;
} date_time;

// TODO: Por qué están comentadas estas funciones?
// extern int get_sec();
// extern int get_min();
// extern int get_hour();
// extern int get_day();
// extern int get_month();
// extern int get_year();
void get_time(date_time *dt);
void timer_handler();
void usleep(uint64_t microsecondsToWait);
void sleep(uint64_t ticksToWait);
void set_pit_frequency(uint32_t frequency);

#endif
