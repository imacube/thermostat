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

  if (get_heat_relay_state() == 1) {
    cout << thermostat.test_messages;
    cout << "\nFAIL: heat relay should not be on!\n";
    return 1;
  }
  return 0;
}

uint8_t test_cool_relay() {
  String messages = "";

  reset_relay_state();
  TestThermostat thermostat = TestThermostat();

  thermostat.set_temp_setting(72);
  thermostat.set_temp(73);
  thermostat.on_cool();
  thermostat.off_heat();
  thermostat.display_state();

  thermostat.display_home();
  thermostat.display_state();

  thermostat.test_messages += "Switching to heat\n";

  thermostat.on_heat();
  thermostat.off_cool();
  thermostat.set_temp_setting(71);
  thermostat.display_home();
  thermostat.display_state();

  set_millis(610000);
  thermostat.display_home();
  thermostat.display_state();

  if (get_cool_relay_state() == 1) {
    cout << thermostat.test_messages;
    cout << "\nFAIL: cool relay should not be on!\n";
    return 1;
  }
  return 0;
}
