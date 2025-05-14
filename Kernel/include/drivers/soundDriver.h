#ifndef _SOUND_DRIVER_H
#define _SOUND_DRIVER_H

#include <stdint.h>

static void play_sound(int nFrequence);
static void no_sound();
void beep(int ticks, int nFrequence);

#endif