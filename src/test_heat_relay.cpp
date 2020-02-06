#include "test_heat_relay.h"

uint8_t test_heat_relay() {
  String messages = "";

  reset_relay_state();
  TestThermostat thermostat = TestThermostat();

  thermostat.set_temp_setting(72);
  thermostat.set_temp(71);
  thermostat.off_cool();
  thermostat.on_heat();
  thermostat.display_state();

  thermostat.display_home();
  thermostat.display_state();

  thermostat.test_messages += "Switching to cool\n";

  thermostat.on_cool();
  thermostat.off_heat();
  thermostat.set_temp_setting(80);
  thermostat.display_home();
  thermostat.display_state();

  set_millis(610000);
  thermostat.display_home();
  thermostat.display_state();

  // thermostat.set_temp(81);
  // thermostat.display_home();
  // display_state(thermostat, messages);

  if (get_heat_relay_state() == 1) {
    cout << "\nFAIL: heat relay should not be on!\n";
    cout << thermostat.test_messages;
    return 1;
  }

  cout << "\nSUCCESS: no errors.\n";
  return 0;
}
