#include <Adafruit_RGBLCDShield.h>
#include <Adafruit_SleepyDog.h>
#include <Arduino.h>
#include <OneWire.h>
#include <Wire.h>
#include <utility/Adafruit_MCP23017.h>

#include "Thermostat.h"

Thermostat::Thermostat() {
  _temp_setting = 70;
  _previous_temp_millis = millis();
  _previous_temp_setting = _temp_setting;
  _current_display = DISPLAY_HOME;
  _refresh = true;

  _fan_mode = FAN_AUTO;
  _fan_relay = OFF;
  _cool = OFF;
  _cool_relay = OFF;
  _heat = OFF;
  _heat_relay = OFF;

  _default_delay = 200;

  #ifdef mock
  #else
  // Set pixels for degree symbol
  _degree[0] = B01100;
  _degree[1] = B10010;
  _degree[2] = B10010;
  _degree[3] = B01100;
  _degree[4] = B00000;
  _degree[5] = B00000;
  _degree[6] = B00000;

  // Set pixels for Smiley face
  // _smiley[0] = B00000;
  // _smiley[1] = B10001;
  // _smiley[2] = B00000;
  // _smiley[3] = B00000;
  // _smiley[4] = B10001;
  // _smiley[5] = B01110;
  // _smiley[6] = B00000;

  // Set pixels for right arrow
  _right_arrow[0] = B00000;
  _right_arrow[1] = B00100;
  _right_arrow[2] = B00010;
  _right_arrow[3] = B11111;
  _right_arrow[4] = B00010;
  _right_arrow[5] = B00100;
  _right_arrow[6] = B00000;
  #endif
}

void Thermostat::init(uint8_t cols, uint8_t rows) {
  Watchdog.enable(8000);
  _lcd = Adafruit_RGBLCDShield();
  _lcd.begin(cols, rows);
  _lcd.createChar(0, _degree);
  // _lcd.createChar(1, _smiley);
  _lcd.createChar(2, _right_arrow);

  _run_stop = millis();
  _idle = millis();
}

void Thermostat::set_display(uint8_t display) {
  /*
  Set the current display
  */
  _current_display = display;
  clear_lcd();
}

void Thermostat::clear_lcd() {
  /*
  Clear the screen and reset the cgursor
  */
  _lcd.clear();
  _lcd.setCursor(0, 0);
}

void Thermostat::display_home() {
  /*
  Display home screen
  */
  #ifdef DEBUG
  Serial.println(F("display_home"));
  Serial.println(_cool_relay);
  Serial.println(_temp);
  #endif

  // Update the display only
  if (_refresh or _previous_temp != _temp or _previous_temp_setting != _temp_setting) {
    _refresh = false;
    _previous_temp = _temp;
    _previous_temp_setting = _temp_setting;

    clear_lcd();
    _lcd.setCursor(0, 1);
    _lcd.print(F("S: "));
    _lcd.print(_temp_setting);
    _lcd.write(byte(0));
    _lcd.print(F("F"));

    _lcd.setCursor(0, 0);
    _lcd.print(F("T: "));
    _lcd.print(_temp);
    _lcd.write(byte(0));
    _lcd.print(F("F"));
    _lcd.print(F(" "));

    if (_cool == ON) {
      _lcd.print(COOL_TEXT);
    }
    else if (_heat == ON) {
      _lcd.print(HEAT_TEXT);
    }

    if (_cool_relay == ON) {
      _lcd.setCursor(8, 1);
      _lcd.print(F("A/C Fan"));
    }
    else if (_heat_relay == ON) {
      _lcd.setCursor(8, 1);
      _lcd.print(F("Heating"));
    }
    else if (_fan_relay == ON) {
      _lcd.setCursor(8, 1);
      _lcd.print(F("Fan"));
    }
  }

  // Actually change the relays
  if (_fan_mode == FAN_AUTO && _cool_relay == OFF && _cool == OFF) {
    fan_relay(false);
  }
  else if (_fan_mode == ON) {
    fan_relay(true);
  }

  if ((_cool == ON && _heat == ON) || (_cool_relay == ON && _heat_relay == ON)) {
    _lcd.print(F("ERROR W/ HVAC"));
    off_heat();
    off_cool();
    shutdown();
    _current_display = DISPLAY_ERROR;
    _error_message = "HEAT and AC turned on at the same time!!!!";
  }
  else if (_cool == ON) {
    if (_temp > _temp_setting) {
      if (_cool_relay == ON || millis() - _run_stop >= MIN_STOP_TIME) {
        fan_relay(true);
        cool_relay(true);
      }
    }
    else {
      if (millis() - _run_start >= MIN_RUN_TIME_COOL) {
        cool_relay(false);
      }
    }
    if (millis() - _run_stop >= POST_AC_FAN && _cool_relay == OFF && _fan_mode == FAN_AUTO) {
      fan_relay(false);
    }
  }
  else if (_heat == ON) {
    if (_temp < _temp_setting) {
      if (_heat_relay == ON || millis() - _run_stop >= MIN_STOP_TIME) {
        heat_relay(true);
      }
    }
    else {
      if (millis() - _run_start >= MIN_RUN_TIME_HEAT) {
        heat_relay(false);
      }
    }
  }
  else if (_heat == OFF && _heat_relay == ON && millis() - _run_start < MIN_RUN_TIME_HEAT) {
    // Wait to turn off the heat relay
  }
  else if (_cool == OFF && _cool_relay == ON && millis() - _run_start < MIN_RUN_TIME_COOL) {
    // Wait to turn off the cool relay
  }
  else {
    off_heat();
    heat_relay(false);
    off_cool();
    cool_relay(false);
  }

  if (_heat == OFF && _heat_relay == ON && millis() - _run_start > MIN_RUN_TIME_HEAT) {
    heat_relay(false);
  }
  if (_cool == OFF && _cool_relay == ON && millis() - _run_stop > MIN_RUN_TIME_COOL) {
    cool_relay(false);
  }

  if (millis() - _idle < IDLE_TIMEOUT) {
    if (_heat == ON) {
      _lcd.setBacklight(RED);
    }
    else if (_cool == ON) {
      _lcd.setBacklight(BLUE);
    }
    else if (_fan_mode == ON) {
      _lcd.setBacklight(VIOLET);
    }
    else {
      _lcd.setBacklight(TEAL);
    }
  }
  else {
    _lcd.setBacklight(LCD_DISPLAYOFF);
  }

  delay(_default_delay);
}

void Thermostat::lcd_blank_portion(uint8_t column, uint8_t line, uint8_t number) {
  /*
    Erase number characters from line,column
  */

  _lcd.setCursor(column, line);
  for (uint8_t i = 0; i < number; i++) {
    _lcd.print(F(" "));
  }
  _lcd.setCursor(column, line);
}

void Thermostat::display_error() {
  /*
  Error display
  */

  static boolean first_run = true; // This isn't intented to ever stop displaying

  if (first_run) {
    first_run = false;
    clear_lcd();
    if (millis() - _idle < IDLE_TIMEOUT) {
      _lcd.setBacklight(YELLOW);
    }
    else {
      _lcd.setBacklight(LCD_DISPLAYOFF);
    }
    _lcd.print(_error_message);
  }

  _lcd.scrollDisplayLeft();
  delay(250);
}

void Thermostat::display_menu() {
  /*
  Displays the menu options for different settings
  */

  static int8_t selected_menu_item = 0;
  static int8_t selected_item = -1;
  const uint16_t select_delay = 2000;
  const uint16_t select_error_delay = 30000;
  static unsigned long exit_timer = 0;

  if (_refresh) {
    _refresh = false;
    exit_timer = millis(); // Reset exit timer

    clear_lcd();
    _lcd.setBacklight(WHITE);
    _lcd.print(F("Menu: "));
    _lcd.print(current_menu_item[selected_menu_item]);
    _lcd.setCursor(0, 1);

    for (uint8_t i = 0; i < selected_menu_count[selected_menu_item]; i++) {

      _lcd.print(F(" "));

      if (selected_menu_item == 0) { // Cool/Heat Menu
        if (selected_item == -1) {
          // Find which item is already chosen before continuing
          if (_hvac_options[i] == HEAT_TEXT && _heat == ON) {
            _lcd.write(byte(2));
            selected_item = i;
          }
          else if (_hvac_options[i] == COOL_TEXT && _cool == ON) {
            _lcd.write(byte(2));
            selected_item = i;
          }
          else if (_hvac_options[i] == OFF_TEXT && _heat == OFF && _cool == OFF) {
            _lcd.write(byte(2));
            selected_item = i;
          }
        }
        else if (selected_item == i) {
          _lcd.write(byte(2));
        }
        _lcd.print(_hvac_options[i]);
      }
      else if (selected_menu_item == 1) { // Fan Menu
        if (selected_item == -1) {
          if (_fan_options[i] == FAN_AUTO_TEXT && _fan_mode == FAN_AUTO) {
            _lcd.write(byte(2));
            selected_item = i;
          }
          else if (_fan_options[i] == FAN_ON_TEXT && _fan_mode == ON) {
            _lcd.write(byte(2));
            selected_item = i;
          }
        }
        else if (selected_item == i) {
          _lcd.write(byte(2));
        }
        _lcd.print(_fan_options[i]);
      }
      else {
        _lcd.print(F("Exit?"));
      }
    }
  }
  else {
    if (millis() - exit_timer >= 30000) {
      // Return to normal display as user is idle
      delay(select_delay);
      _current_display = DISPLAY_HOME;
      _refresh = true;
      selected_item = -1;
      selected_menu_item = 0;
      return;
    }
  }

  uint8_t buttons = _lcd.readButtons();
  if (buttons) {
    _refresh = true;
    if (buttons & BUTTON_UP) {
      selected_menu_item = selected_menu_item_math(selected_menu_item - 1);
      selected_item = -1;
      delay(_default_delay);
      return;
    }
    else if (buttons & BUTTON_DOWN) {
      selected_menu_item = selected_menu_item_math(selected_menu_item + 1);
      selected_item = -1;
      delay(_default_delay);
      return;
    }
    else if (buttons & BUTTON_SELECT) {
      _lcd.setBacklight(GREEN);
      if (selected_menu_item == 0) { // hvac options
        if (_hvac_options[selected_item] == COOL_TEXT) {
          on_cool();
        }
        else if (_hvac_options[selected_item] == OFF_TEXT) {
          if (_cool == OFF && _heat == OFF)
          {
            heat_relay(false);
            cool_relay(false);
          }
          off_heat();
          off_cool();
        }
        else if (_hvac_options[selected_item] == HEAT_TEXT) {
          on_heat();
        }
        else {
          _lcd.setBacklight(RED);
          delay(select_error_delay);
        }
      }
      else if (selected_menu_item == 1) { // fan options
        if (_fan_options[selected_item] == FAN_AUTO_TEXT) {
          auto_fan();
        }
        else if (_fan_options[selected_item] == FAN_ON_TEXT) {
          on_fan();
        }
        else {
          _lcd.setBacklight(RED);
          delay(select_error_delay);
        }
      }
      delay(select_delay);
      _current_display = DISPLAY_HOME;
      _refresh = true;
      selected_item = -1;
      selected_menu_item = 0;
      return;
    }
    else if (buttons & BUTTON_RIGHT) {
      selected_item += 1;
    }
    else if (buttons & BUTTON_LEFT) {
      selected_item -= 1;
    }

    if (selected_item > selected_menu_count[selected_menu_item] - 1) {
      selected_item = 0;
    }
    else if (selected_item < 0) {
      selected_item = selected_menu_count[selected_menu_item] - 1;
    }

    selected_menu_item = selected_menu_item_math(selected_menu_item);
  }
  delay(_default_delay);
}

void Thermostat::auto_fan() {
  _fan_mode = FAN_AUTO;
}

void Thermostat::on_fan() {
  _fan_mode = ON;
}

void Thermostat::off_heat() {
  _heat = OFF;
}

void Thermostat::off_cool() {
  _cool = OFF;
}

void Thermostat::on_heat() {
  _cool = OFF;
  _heat = ON;
  _run_stop = millis() - MIN_STOP_TIME;
}

void Thermostat::on_cool() {
  _heat = OFF;
  _cool = ON;
  _run_stop = millis() - MIN_STOP_TIME;
}

int8_t Thermostat::selected_menu_item_math(int8_t selected_menu_item) {
  /*
  This simple if logic is used multiple times so a function was made
  */

  if (selected_menu_item > CURRENT_MENU_ITEM_COUNT - 1) return 0;
  else if (selected_menu_item < 0) return CURRENT_MENU_ITEM_COUNT - 1;
  else return selected_menu_item;
}

void Thermostat::set_temp(uint8_t temp) {
  /*
  Set the current temperature
  */

  _previous_temp = _temp;
  _temp = temp;
  _previous_temp_millis = millis(); // Update temp millis to reset time since last updated
}

void Thermostat::set_temp_setting(uint8_t temp_setting) {
  /*
  Set the temperature setting
  */

  _temp_setting = temp_setting;
}

void Thermostat::test() {
  /*
    For testing only
  */

  _lcd.setBacklight(VIOLET);
  _lcd.print(F("Temp: "));
  _lcd.print(_temp);
  _lcd.write(byte(0));
  _lcd.print(F("F"));
  _lcd.setCursor(0, 1);
  _lcd.print(F("Set: "));
  _lcd.write(byte(1));

}

void Thermostat::saftey_check() {
  /*
  Run a saftey/sanity check
  */
  static boolean saftey_check_triggered = false; // True if something went wrong

  if (millis() - _previous_temp_millis >= PREVIOUS_TEMP_INTERVAL) {
    // If no temp reported with in proper window, turn off
    shutdown();
    _current_display = DISPLAY_ERROR;
    _error_message = "No temp set within acceptable interval";
    saftey_check_triggered = true;
  }
  else if (_temp < TEMP_LOW || _temp > TEMP_HIGH) {
    // If temp change too extreme turn off
    shutdown();
    _current_display = DISPLAY_ERROR;
    _error_message = "Temp outside of low/high range: ";
    _error_message += _temp;
    saftey_check_triggered = true;
  }
  else if (saftey_check_triggered) {
    saftey_check_triggered = false;
    _current_display = DISPLAY_HOME;
  }
}

void Thermostat::refresh() {
  /*
  Trigger a refresh
  */
  _refresh = true;
}

void Thermostat::yield() {
  /*
  Yield control to the Thermostat object
  */
  uint8_t buttons = _lcd.readButtons();

  Watchdog.reset();

  // Add saftey code here, e.g.
  saftey_check();

  static unsigned long _previous_temp_millis_monitor = 0;
  if (_previous_temp_millis_monitor != _previous_temp_millis) {
    #ifdef DEBUG
    Serial.print(F("_previous_temp_millis: "));
    Serial.print(_previous_temp_millis);
    Serial.print(F(" : "));
    Serial.println(_previous_temp_millis - _previous_temp_millis_monitor);
    #endif
    _previous_temp_millis_monitor = _previous_temp_millis;
  }

  if (_current_display == DISPLAY_HOME) {
    buttons = _lcd.readButtons();
    if (buttons) {
      _idle = millis();
      if (buttons & BUTTON_SELECT) {
        _current_display = DISPLAY_MENU;
        _refresh = 1;
      }
      else if (buttons & BUTTON_UP) set_temp_setting(_temp_setting + (uint8_t) 1);
      else if (buttons & BUTTON_DOWN) set_temp_setting(_temp_setting - (uint8_t) 1);
      delay(_default_delay);
    } else {
      display_home();
    }
  }
  else if (_current_display == DISPLAY_MENU) {
    display_menu();
  }
  else if (_current_display == DISPLAY_ERROR) {
    // Something bad has happened, the system will be locked until the board is reset
    display_error();
  }
  else if (_current_display == DISPLAY_OVERRIDE) {
    // Just a placeholder
  }
}

void Thermostat::fan_relay(boolean mode = false) {
  /*
    Turn on FAN

    mode: true ON, false OFF
  */
  static boolean started = false; // When false set _refresh = true
  if (mode) { // If told to turn on
    if (!started) {
      started = !started;
      _refresh = true;
    }
    _fan_relay = ON;
    digitalWrite(RELAY_FAN, LOW);
  }
  else { // Else turn off
    if (started) {
      started = !started;
      _refresh = true;
    }
    digitalWrite(RELAY_FAN, HIGH);
    _fan_relay = OFF;
  }
}

void Thermostat::cool_relay(boolean mode = false) {
  /*
    Turn on Cool

    mode: true ON, false OFF
  */
  static boolean started = false; // When false set _run_start
  if (mode) { // If told to turn on
    if (!started) {
      _run_start = millis();
      started = !started;
      _refresh = true;
      _temp_start = _temp;
    }
    off_heat();
    heat_relay(false);
    _cool_relay = ON;
    digitalWrite(RELAY_COOL_1, LOW);
    digitalWrite(RELAY_COOL_2, LOW);
  }
  else { // Else turn off
    if (started) {
      _run_stop = millis();
      started = !started;
      _refresh = true;
    }
    digitalWrite(RELAY_COOL_1, HIGH);
    digitalWrite(RELAY_COOL_2, HIGH);
    _cool_relay = OFF;
  }
}

void Thermostat::heat_relay(boolean mode = false) {
  /*
    Turn on Heat

    mode: true ON, false OFF
  */
  static boolean started = false; // When false set _run_start
  if (mode) { // If told to turn on
    if (!started) {
      #ifdef DEBUG
      Serial.println(F("heat_relay"));
      #endif
      _run_start = millis();
      started = !started;
      _refresh = true;
      _temp_start = _temp;
    }
    off_cool();
    cool_relay(false);
    _heat_relay = ON;
    digitalWrite(RELAY_HEAT, LOW);
  }
  else { // Else turn off
    if (started) {
      _run_stop = millis();
      started = !started;
      _refresh = true;
    }
    digitalWrite(RELAY_HEAT, HIGH);
    _heat_relay = OFF;
  }
}

void Thermostat::shutdown() {
  /*
  Turn everything off
  */
  fan_relay(false);
  cool_relay(false);
  heat_relay(false);
}

uint8_t Thermostat::get_set_temp() {
  /*
    Return the set temperature.
  */

  return _temp_setting;
}

uint8_t Thermostat::get_temp() {
  /*
    Return the temperature.
  */

  return _temp;
}

uint8_t Thermostat::get_cool() {
  /*
    Return cool relay state.
  */

  return _cool;
}

uint8_t Thermostat::get_heat() {
  /*
    Return heat relay state.
  */

  return _heat;
}
