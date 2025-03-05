#include <time.h>
#include <time_rtc.h>
#include <stdint.h>
#include <naiveConsole.h>
#include <videodriver.h>
#include <keyboardDriver.h>


static void int_20();
static void int_21();
static void (*interrupt_arr[])(void) = {int_20, int_21};

void irqDispatcher(uint64_t irq) {

	interrupt_arr[irq]();
}

void int_20() {
	timer_handler();

}

void int_21(){
	press_key(); 
}
