#include <iostream>

// #include <Arduino.h>

// #include "add.h"
#include "Thermostat.h"

using std::cout;

class TestThermostat: public Thermostat
{
  public:
    uint8_t heat_relay_state = 0;
    uint8_t cool_relay_state = 0;

    void off_cool_relay() {
      cool_relay_state = 0;
      cout << "Cool Relay Off\n";
    }
    void on_cool_relay() {
      cool_relay_state = 1;
      cout << "Cool Relay On\n";
    }
    void off_heat_relay() {
      heat_relay_state = 0;
      cout << "Heat Relay Off\n";
    }
    void on_heat_relay() {
      heat_relay_state = 1;
      cout << "Heat Relay On\n";
    }

    void display_home() {
      cout << "Calling display home\n";
      Thermostat::display_home();
    }
};

void display_state(TestThermostat thermostat);

int main() {
  cout << "\nBegin Test Output\n\n";

  TestThermostat thermostat = TestThermostat();

  thermostat.set_temp_setting(72);
  thermostat.set_temp(71);
  thermostat.on_heat();
  display_state(thermostat);

  thermostat.display_home();
  display_state(thermostat);

  thermostat.on_cool();
  thermostat.set_temp(80);
  thermostat.display_home();
  display_state(thermostat);

  cout << "\nEnd Test Output\n";
  return 0;
}

void display_state(TestThermostat thermostat) {
  cout << "\nDisplay State\n";
  printf("Temp\t\t\t%d\n", thermostat.get_temp());
  printf("Heat\t\t\t%d\n", thermostat.get_heat());
  printf("Cool\t\t\t%d\n", thermostat.get_cool());
  printf("Heat relay state\t%d\n", thermostat.heat_relay_state);
  printf("Cool relay state\t%d\n", thermostat.cool_relay_state);
  cout << "\n";
}
