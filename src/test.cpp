#include <iostream>

// #include <Arduino.h>

// #include "add.h"
#include "Thermostat.h"

using std::cout;

int main() {
  cout << "\nBegin Test Output\n";

  Thermostat thermostat = Thermostat();

  thermostat.set_temp_setting(72);
  thermostat.set_temp(72);

  printf("%d\n", thermostat.get_temp());

  cout << "End Test Output\n";
  return 0;
}
