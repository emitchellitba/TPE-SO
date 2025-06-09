#include <memory_manager.h>
#include <process.h>
#include <queue.h>
#include <scheduler.h>
#include <stdint.h>
#include <time_rtc.h>

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40

KMEMORY_DECLARE

extern void _hlt();
extern void outb(uint8_t PORT, uint8_t n);
extern int bcd_to_dec(int num);
extern int get_sec();
extern int get_min();
extern int get_hour();
extern int get_day();
extern int get_month();
extern int get_year();

// static int frequency = 18;

typedef struct {
  pid_t pid;
  unsigned long wake_up_tick;
} sleeping_proc_t;

static uint64_t tick_frequency = 18;  // Frecuencia del PIT en Hz (18.2 Hz por defecto)

static struct queue *sleeping_proc_queue = QUEUE_NEW();

unsigned long ticks = 0; // Variable global para contar los ticks del PIT

// Manejador de interrupción del PIT
void timer_handler() {
  ticks++;

  struct qnode *next;
  queue_for(node, sleeping_proc_queue) {
    // Save next node in case we remove the current one
    next = node->next;
    sleeping_proc_t *sp = (sleeping_proc_t *)node->value;
    if (sp->wake_up_tick <= ticks) {
      unblock_process_by_pid(sp->pid);
      queue_node_remove(sleeping_proc_queue, node);
      kmm_free(sp, kernel_mem);
    }
  }
}

uint64_t seconds_to_ticks(uint64_t seconds) {
    return seconds * tick_frequency;
}

uint64_t ticks_to_seconds(uint64_t ticks) {
    return ticks / tick_frequency;
}

void sleep_helper(uint64_t ticksToWait) {
  sleeping_proc_t *current = kmalloc(kernel_mem, sizeof(sleeping_proc_t));
  current->pid = get_running()->pid;
  current->wake_up_tick = ticks + ticksToWait;
  enqueue(sleeping_proc_queue, current);
  block_current(BLK_SLEEP, sleeping_proc_queue);
}

void usleep(uint64_t microsecondsToWait) {
  uint64_t ticksToWait = (seconds_to_ticks(microsecondsToWait)) / 1000000;
  if (ticksToWait == 0) {
    return;
  }
  sleep_helper(ticksToWait);
}

void sleep(uint64_t secondsToWait) {
  sleep_helper(seconds_to_ticks(secondsToWait));
}

void set_pit_frequency(uint32_t frequency) {
  uint16_t divisor = 1193180 / frequency;

  outb(PIT_COMMAND_PORT, 0x36);

  outb(PIT_CHANNEL0_PORT, (uint8_t)(divisor & 0xFF));
  outb(PIT_CHANNEL0_PORT, (uint8_t)((divisor >> 8) & 0xFF));

  tick_frequency = frequency;
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
