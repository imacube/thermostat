#include "test_heat_relay.h"

void test_heat_relay() {
  reset_relay_state();
  TestThermostat thermostat = TestThermostat();

  thermostat.set_temp_setting(72);
  thermostat.set_temp(71);
  thermostat.off_cool();
  thermostat.on_heat();
  display_state(thermostat);
  thermostat.display_home();
  display_state(thermostat);

  cout << "Switching to cool\n";

  thermostat.on_cool();
  thermostat.off_heat();
  thermostat.set_temp_setting(80);
  thermostat.display_home();
  display_state(thermostat);

  set_millis(610000);
  thermostat.display_home();
  display_state(thermostat);

  // thermostat.set_temp(81);
  // thermostat.display_home();
  // display_state(thermostat);

  if (get_heat_relay_state() == 1) {
    cout << "\nFAIL: heat relay should not be on!\n";
  }
  else {
    cout << "\nSUCCESS: no errors.\n";
  }
}
