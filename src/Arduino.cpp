#include "Arduino.h"

unsigned long arduino_current_millis = 0;

String F(String msg) {
    return msg;
}

uint8_t byte(uint8_t int_to_byte) {
  return int_to_byte;
}

void delay(uint32_t ms) {
  arduino_current_millis += ms;
}

unsigned long millis() {
  return arduino_current_millis;
}

void pinMode(uint8_t pin, uint8_t mode) {}
