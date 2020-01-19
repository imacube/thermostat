#include <iostream>

// #include "Thermostat.h"

#include <Arduino.h>
#include <MockSerial.h>
// #include "Arduino.h"
// #include "add.h"

int main() {
  std::cout << "hello world!\n";
  std::cout << millis();
  Serial.begin(9600);
  Serial.write("Foobar");
  // std::cout << add(1, 2) << "\n";
  return 0;
}
