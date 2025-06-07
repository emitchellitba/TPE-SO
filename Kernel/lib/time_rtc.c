#include <stdint.h>
#include <time_rtc.h>
#include <scheduler.h>
#include <process.h>
#include <memory_manager.h>
#include <queue.h>

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

void sleep(int ticksToWait) {
  sleeping_proc_t* current = kmalloc(kernel_mem, sizeof(sleeping_proc_t));
  current->pid = get_running()->pid;
  current->wake_up_tick = ticks + ticksToWait;
  enqueue(sleeping_proc_queue, current);
  block_current(BLK_SLEEP, NULL);
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
