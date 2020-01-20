#include "Arduino.h"

String F(String msg) {
    return msg;
}

uint8_t byte(uint8_t int_to_byte) {
  return int_to_byte;
}

void delay(uint32_t ms) {
  // printf("Asked to delay %d ms\n", ms);
}

unsigned long millis() {
  return 0;
}

void pinMode(uint8_t pin, uint8_t mode) {}

void digitalWrite(uint8_t pin, uint8_t state) {}
