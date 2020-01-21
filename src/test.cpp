#include "test.h"

int main() {
  cout << "\nBegin Test Output\n\n";

  TestThermostat thermostat = TestThermostat();

  thermostat.set_temp_setting(72);
  thermostat.set_temp(71);
  thermostat.on_heat();
  display_state(thermostat);

  thermostat.display_home();
  display_state(thermostat);

  thermostat.off_heat();
  thermostat.on_cool();
  thermostat.set_temp_setting(80);
  thermostat.display_home();
  display_state(thermostat);

  thermostat.set_temp(82);
  thermostat.display_home();
  display_state(thermostat);

  cout << "\nEnd Test Output\n";
  return 0;
}

void display_state(TestThermostat thermostat) {
  cout << "\nDisplay State\n";

  printf("Set Temp\t\t%d\n", thermostat.get_set_temp());
  printf("Temp\t\t\t%d\n", thermostat.get_temp());
  printf("Heat\t\t\t%d\n", thermostat.get_heat());
  printf("Cool\t\t\t%d\n", thermostat.get_cool());
  printf("Heat relay state\t%d\n", heat_relay_state);
  printf("Cool relay state\t%d\n", cool_relay_state);
  printf("Fan relay state\t\t%d\n", fan_relay_state);
  cout << "\n";
}

void digitalWrite(uint8_t pin, uint8_t value) {
  if (pin == RELAY_HEAT) {
    if (value == HIGH) {
      heat_relay_state = 1;
    }
    else {
      heat_relay_state = 0;
    }
  }
  else if (pin == RELAY_COOL) {
    if (value == HIGH) {
      cool_relay_state = 1;
    }
    else {
      cool_relay_state = 0;
    }
  }
  else if (pin == RELAY_FAN) {
    if (value == HIGH) {
      fan_relay_state = 1;
    }
    else {
      fan_relay_state = 0;
    }
  }
}

void TestThermostat::off_cool_relay() {
  cout << "Cool Relay Off\n";
  Thermostat::off_cool_relay();
}
void TestThermostat::on_cool_relay() {
  cout << "Cool Relay On\n";
  Thermostat::on_cool_relay();
}
void TestThermostat::off_heat_relay() {
  cout << "Heat Relay Off\n";
  Thermostat::off_heat_relay();
}
void TestThermostat::on_heat_relay() {
  cout << "Heat Relay On\n";
  Thermostat::on_heat_relay();
}

void TestThermostat::display_home() {
  cout << "Calling display home\n";
  Thermostat::display_home();
}
