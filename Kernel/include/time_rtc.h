#ifndef _TIME_H_
#define _TIME_H_
#include<stdint.h>

typedef struct{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint8_t year;
}date_time;
    
// extern int getSec();
// extern int getMin();
// extern int getHour();
// extern int getDay();
// extern int getMonth();
// extern int getYear();
void get_time(date_time *dt);
void timer_handler();
int ticks_elapsed();
int seconds_elapsed();
void sleep(int ticksToWait);

#endif
