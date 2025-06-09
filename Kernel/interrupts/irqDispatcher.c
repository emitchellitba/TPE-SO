// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java:
// https://pvs-studio.com

#include <keyboardDriver.h>
#include <stdint.h>
#include <time_rtc.h>

static void int_20();
static void int_21();
static void (*interrupt_arr[])(void) = {int_20, int_21};

void irqDispatcher(uint64_t irq) { interrupt_arr[irq](); }

void int_20() { timer_handler(); }

void int_21() { handle_key_press(); }
