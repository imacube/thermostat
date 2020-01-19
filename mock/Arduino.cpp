#include "Arduino.h"

unsigned long millis() {
  unsigned char f;
  f = 0b111;
  return 0;
}

void pinMode(uint8_t pin, uint8_t mode) {}

void digitalWrite(uint8_t pin, uint8_t state) {}
