#ifndef test_helpers_h
#define test_helpers_h

#include <iostream>

#include "Thermostat.h"

using std::cout;

uint8_t get_heat_relay_state();
uint8_t get_cool_relay_state();
uint8_t get_fan_relay_state();
void reset_relay_state();

class TestThermostat: public Thermostat
{
  public:
    void cool_relay(boolean);
    void heat_relay(boolean);
    void display_home();
};

void display_state(TestThermostat);

#endif
