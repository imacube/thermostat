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
    test_messages += "Cool Relay Off\n";
  }
  else if (mode == true) {
    test_messages += "Cool Relay On\n";
  }
  else {
    test_messages += "Cool Relay mode " + to_string(mode) + "\n";
  }
  Thermostat::cool_relay(mode);
}
void TestThermostat::heat_relay(boolean mode) {
  if (mode == false) {
    test_messages += "Heat Relay Off\n";
  }
  else if (mode == true) {
    test_messages += "Heat Relay On\n";
  }
  else {
    test_messages += "Heat Relay mode " + to_string(mode) + "\n";
  }
  Thermostat::heat_relay(mode);
}
void TestThermostat::display_home() {
  test_messages += "Calling display home\n";
  Thermostat::display_home();
}

void TestThermostat::display_state() {
  test_messages += "\nDisplay State\n";
  test_messages += "millis\t\t\t" + to_string(millis()) + "\n";
  test_messages += "Set Temp\t\t" + to_string(get_set_temp()) + "\n";
  test_messages += "Temp\t\t\t" + to_string(get_temp()) + "\n";
  test_messages += "Heat\t\t\t" + to_string(get_heat()) + "\n";
  test_messages += "Cool\t\t\t" + to_string(get_cool()) + "\n";
  test_messages += "Heat relay state\t" + to_string(_heat_relay) + "\n";
  test_messages += "Cool relay state\t" + to_string(_cool_relay) + "\n";
  test_messages += "Fan relay state\t\t" + to_string(_fan_relay) + "\n";
  test_messages += "\n";
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
