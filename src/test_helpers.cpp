#include "test_helpers.h"

uint8_t heat_relay_state = 0;
uint8_t cool_relay_state = 0;
uint8_t fan_relay_state = 0;

uint8_t get_heat_relay_state() {
  return heat_relay_state;
}

uint8_t get_cool_relay_state() {
  return cool_relay_state;
}

uint8_t get_fan_relay_state() {
  return fan_relay_state;
}

void reset_relay_state() {
  heat_relay_state = 0;
  cool_relay_state = 0;
  fan_relay_state = 0;
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

void display_state(TestThermostat thermostat) {
  cout << "\nDisplay State\n";

  printf("millis\t\t\t%lu\n", millis());
  printf("Set Temp\t\t%d\n", thermostat.get_set_temp());
  printf("Temp\t\t\t%d\n", thermostat.get_temp());
  printf("Heat\t\t\t%d\n", thermostat.get_heat());
  printf("Cool\t\t\t%d\n", thermostat.get_cool());
  printf("Heat relay state\t%d\n", thermostat._heat_relay);
  printf("Cool relay state\t%d\n", thermostat._cool_relay);
  printf("Fan relay state\t\t%d\n", thermostat._fan_relay);
  cout << "\n";
}

void digitalWrite(uint8_t pin, uint8_t value) {
  if (pin == RELAY_HEAT) {
    if (value == LOW) {
      heat_relay_state = 1;
    }
    else {
      heat_relay_state = 0;
    }
  }
  else if (pin == RELAY_COOL_1 || pin == RELAY_COOL_2) {
    if (value == LOW) {
      cool_relay_state = 1;
    }
    else {
      cool_relay_state = 0;
    }
  }
  else if (pin == RELAY_FAN) {
    if (value == LOW) {
      fan_relay_state = 1;
    }
    else {
      fan_relay_state = 0;
    }
  }
}
