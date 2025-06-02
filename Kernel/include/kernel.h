#ifndef KERNEL_H
#define KERNEL_H

#include "heap.h"
#include <ds/queue.h>
#include <exec.h>
#include <fs.h>
#include <idtLoader.h>
#include <keyboardDriver.h>
#include <lib.h>
#include <logger.h>
#include <memory_manager.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <proc/process.h>
#include <scheduler.h>
#include <semaphores/semaphore.h>
#include <soundDriver.h>
#include <stdint.h>
#include <string.h>
#include <videodriver.h>

extern int init_main(int argc, char **argv);
extern void call_timer_tick();
extern void _cli();

#endif // KERNEL_H
