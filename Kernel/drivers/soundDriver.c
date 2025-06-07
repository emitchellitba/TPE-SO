#include <lib.h>
#include <soundDriver.h>
#include <stdint.h>

// TODO: Deprecated use of sleep

extern void outb(uint8_t a, uint8_t tmp);
extern uint8_t inb(uint8_t a);
extern void sleep(int ticks);

// Play sound using built-in speaker
static void play_sound(int nFrequence) {
  uint32_t Div;
  uint8_t tmp;

  // Set the PIT to the desired frequency
  Div = 1193180 / nFrequence;
  outb((uint8_t)0x43, 0xb6);
  outb((uint8_t)0x42, (uint8_t)(Div));
  outb((uint8_t)0x42, (uint8_t)(Div >> 8));

  // And play the sound using the PC speaker
  tmp = inb((uint8_t)0x61);
  if (tmp != (tmp | 3)) {
    outb((uint8_t)0x61, tmp | 3);
  }
}

// make it shut up
static void no_sound() {
  uint8_t tmp = inb((uint8_t)0x61) & 0xFC;

  outb((uint8_t)0x61, tmp);
}

// Make a beep
void beep(int ticks, int nFrequence) {
  play_sound(nFrequence);
  sleep(ticks);
  no_sound();
}