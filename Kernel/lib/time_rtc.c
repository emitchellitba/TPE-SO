#include <stdint.h>
#include <time_rtc.h>

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40

extern void _hlt();
extern void outb(int PORT, int n);
extern int bcd_to_dec(int num);
extern int get_sec();
extern int get_min();
extern int get_hour();
extern int get_day();
extern int get_month();
extern int get_year();

// static int frequency = 18;

unsigned long ticks = 0; // Variable global para contar los ticks del PIT

// Manejador de interrupción del PIT
void timer_handler() { ticks++; }

void sleep(int ticksToWait) {
  unsigned long start_time = ticks;
  while ((ticks - start_time) < ticksToWait) {
    _hlt();
  };
}

void set_pit_frequency(uint32_t frequency) {
  uint16_t divisor = 1193180 / frequency;

  outb(PIT_COMMAND_PORT, 0x36);

  outb(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));
  outb(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));
}

void get_time(date_time *dt) {
  dt->sec = bcd_to_dec(get_sec());
  dt->min = bcd_to_dec(get_min());
  dt->hour = bcd_to_dec(get_hour());
  dt->day = bcd_to_dec(get_day());
  dt->month = bcd_to_dec(get_month());
  dt->year = bcd_to_dec(get_year());
}

int bcd_to_dec(int num) { return ((num >> 4) * 10) + (num & 0x0F); }
