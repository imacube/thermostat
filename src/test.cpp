#include <iostream>

#include <Arduino.h>

// #include "add.h"
// #include "Thermostat.h"

int main() {
  std::cout << "hello world!\n";
  std::cout << millis();
  Serial.begin(9600);
  Serial.write("Foobar\n");
  // std::cout << add(1, 2) << "\n";
  delay((uint8_t) 123);
  return 0;
}
