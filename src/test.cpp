#include "test.h"

int main() {
  cout << "\nBegin Test Output\n\n";

  TestThermostat thermostat = TestThermostat();

  thermostat.set_temp_setting(72);
  thermostat.set_temp(71);
  thermostat.off_cool();
  thermostat.on_heat();
  display_state(thermostat);

  thermostat.display_home();
  display_state(thermostat);

  thermostat.on_cool();
  thermostat.off_heat();
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

  printf("millis\t\t\t%lu\n", millis());
  printf("Set Temp\t\t%d\n", thermostat.get_set_temp());
  printf("Temp\t\t\t%d\n", thermostat.get_temp());
  printf("Heat\t\t\t%d\n", thermostat.get_heat());
  printf("Cool\t\t\t%d\n", thermostat.get_cool());
  printf("Heat relay state\t%d\n", heat_relay_state);
  printf("Cool relay state\t%d\n", cool_relay_state);
  printf("Fan relay state\t\t%d\n", fan_relay_state);
  cout << "\n";
}

void TestThermostat::cool_relay(boolean mode) {
  if (mode == false) {
    cout << "Cool Relay Off\n";
  }
  else if (mode == true) {
    cout << "Cool Relay On\n";
  }
  else {
    cout << "Cool Relay mode " << mode << "\n";
  }
  Thermostat::cool_relay(mode);
}
void TestThermostat::heat_relay(boolean mode) {
  if (mode == false) {
    cout << "Heat Relay Off\n";
  }
  else if (mode == true) {
    cout << "Heat Relay On\n";
  }
  else {
    cout << "Heat Relay mode " << mode << "\n";
  }
  Thermostat::heat_relay(mode);
}

void TestThermostat::display_home() {
  cout << "Calling display home\n";
  Thermostat::display_home();
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
  else if (pin == RELAY_COOL_1 || pin == RELAY_COOL_2) {
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
