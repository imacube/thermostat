#ifndef test_h
#define test_h

#include <iostream>

#include "Thermostat.h"

using std::cout;

uint8_t heat_relay_state = 0;
uint8_t cool_relay_state = 0;
uint8_t fan_relay_state = 0;

class TestThermostat: public Thermostat
{
  public:
    void off_cool_relay();
    void on_cool_relay();
    void off_heat_relay();
    void on_heat_relay();
    void display_home();
};

void display_state(TestThermostat thermostat);

#endif
