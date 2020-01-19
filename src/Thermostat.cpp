#include <Arduino.h>
#include <OneWire.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <Serial.h>
#include "Thermostat.h"

Thermostat thermostat = Thermostat();

// OneWire ds(10);  // on pin 10 (a 4.7K resistor is necessary); this is the temperature sensor!
// byte addr[8]; // Address of temperature sensor

void setup(void) {
  Serial.begin(9600);
  thermostat.init((uint8_t) 16, (uint8_t) 2);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_COOL, OUTPUT);
  pinMode(RELAY_HEAT, OUTPUT);

  thermostat.set_temp((uint8_t) 69);
  thermostat.display_home();

}

void loop(void) {
  thermostat.yield();
}

Thermostat::Thermostat() {
  _temp_setting = 70;
  _previous_temp_setting = _temp_setting;
  _lcd_backlight_color = WHITE;
  _current_display = DISPLAY_HOME;
  _refresh = true;

  _fan_mode = FAN_AUTO;
  _fan_state = OFF;
  _cool = OFF;
  _heat = OFF;

  _default_delay = 200;

  // Set pixels for degree symbol
  _degree[0] = B01100;
  _degree[1] = B10010;
  _degree[2] = B10010;
  _degree[3] = B01100;
  _degree[4] = B00000;
  _degree[5] = B00000;
  _degree[6] = B00000;

  // Set pixels for Smiley face
  _smiley[0] = B00000;
  _smiley[1] = B10001;
  _smiley[2] = B00000;
  _smiley[3] = B00000;
  _smiley[4] = B10001;
  _smiley[5] = B01110;
  _smiley[6] = B00000;

  // Set pixels for right arrow
  _right_arrow[0] = B00000;
  _right_arrow[1] = B00100;
  _right_arrow[2] = B00010;
  _right_arrow[3] = B11111;
  _right_arrow[4] = B00010;
  _right_arrow[5] = B00100;
  _right_arrow[6] = B00000;
}

void Thermostat::init(uint8_t cols, uint8_t rows) {
  _lcd = Adafruit_RGBLCDShield();
  _lcd.begin(cols, rows);
  _lcd.createChar(0, _degree);
  _lcd.createChar(1, _smiley);
  _lcd.createChar(2, _right_arrow);
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

  if (_refresh or _previous_temp != _temp or _previous_temp_setting != _temp_setting) {
    _refresh = false;
    _previous_temp = _temp;
    _previous_temp_setting = _temp_setting;

    clear_lcd();
    _lcd.print(F("T: "));
    _lcd.print(_temp);
    _lcd.write(byte(0));
    _lcd.print(F("F"));
    _lcd.print(F(" "));

    if (_cool == ON && _heat == ON) {
      _lcd.print(F("ERROR W/ HVAC"));
      off_heat();
      off_cool();
    }
    else if (_cool == ON) {
      _lcd.print(_cool_text);
      if (_temp > _temp_setting) {
        on_cool_relay();
      }
      else {
        off_cool_relay();
      }
    }
    else if (_heat == ON) {
      _lcd.print(_heat_text);
      if (_temp < _temp_setting) {
        on_heat_relay();
      }
      else {
        off_heat_relay();
      }
    }
    else {
      off_heat_relay();
      off_cool_relay();
    }

    _lcd.setCursor(0, 1);
    _lcd.print(F("S: "));
    _lcd.print(_temp_setting);
    _lcd.write(byte(0));
    _lcd.print(F("F"));
    _lcd.print(F(" "));

    if (_fan_mode == FAN_AUTO) {
      _lcd.print(F("Fan Auto"));
    }
    else if (_fan_mode == FAN_ON) {
      _lcd.print(F("Fan On"));
      on_fan_relay();
    }

    set_backlight();
  }

  delay(_default_delay);
}

void Thermostat::fan_auto() {
  _fan_mode = FAN_AUTO;
}

void Thermostat::fan_on() {
  _fan_mode = FAN_ON;
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

void Thermostat::display_menu() {
  /*
  Displays the menu options for different settings
  */

  static int8_t selected_menu_item = 0;
  static int8_t selected_item = -1;
  const uint16_t select_delay = 2000;
  const uint16_t select_error_delay = 30000;
  static uint32_t exit_timer = 0;

  if (_refresh) {
    _refresh = false;
    exit_timer = millis(); // Reset exit timer

    clear_lcd();
    _lcd.setBacklight(VIOLET);
    _lcd.print(F("Menu: "));
    _lcd.print(current_menu_item[selected_menu_item]);
    _lcd.setCursor(0, 1);

    for (uint8_t i = 0; i < selected_menu_count[selected_menu_item]; i++) {

      _lcd.print(F(" "));

      if (selected_menu_item == 0) { // Cool/Heat Menu
        if (selected_item == -1) {
          // Find which item is already chosen before continuing
          if (_hvac_options[i] == _heat_text && _heat == ON) {
            _lcd.write(byte(2));
            selected_item = i;
          }
          else if (_hvac_options[i] == _cool_text && _cool == ON) {
            _lcd.write(byte(2));
            selected_item = i;
          }
          else if (_hvac_options[i] == _off_text && _heat == OFF && _cool == OFF) {
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
          if (_fan_options[i] == _fan_auto_text && _fan_mode == FAN_AUTO) {
            _lcd.write(byte(2));
            selected_item = i;
          }
          else if (_fan_options[i] == _fan_on_text && _fan_mode == FAN_ON) {
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
    if (millis() - exit_timer > 30000 || millis() < exit_timer) {
      // Either more then 30 seconds have elapsed or the timer has rolled over
      // so return to the normal display
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
    if ((buttons & BUTTON_UP) == BUTTON_UP) {
      selected_menu_item = selected_menu_item_math(selected_menu_item - 1);
      selected_item = -1;
      delay(_default_delay);
      return;
    }
    else if ((buttons & BUTTON_DOWN) == BUTTON_DOWN) {
      selected_menu_item = selected_menu_item_math(selected_menu_item + 1);
      selected_item = -1;
      delay(_default_delay);
      return;
    }
    else if ((buttons & BUTTON_SELECT) == BUTTON_SELECT) {
      _lcd.setBacklight(GREEN);
      if (selected_menu_item == 0) { // hvac options
        if (_hvac_options[selected_item] == _cool_text) {
          on_cool();
        }
        else if (_hvac_options[selected_item] == _off_text) {
          off_heat();
          off_cool();
        }
        else if (_hvac_options[selected_item] == _heat_text) {
          on_heat();
        }
        else {
          _lcd.setBacklight(RED);
          delay(select_error_delay);
        }
      }
      else if (selected_menu_item == 1) { // fan options
        if (_fan_options[selected_item] == _fan_auto_text) {
          fan_auto();
        }
        else if (_fan_options[selected_item] == _fan_on_text) {
          fan_on();
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
    else if ((buttons & BUTTON_RIGHT) == BUTTON_RIGHT) {
      selected_item += 1;
    }
    else if ((buttons & BUTTON_LEFT) == BUTTON_LEFT) {
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

void Thermostat::off_heat() {
  _heat = OFF;
  if (_fan_mode == FAN_AUTO) _fan_state = OFF;
}

void Thermostat::off_cool() {
  _cool = OFF;
  if (_fan_mode == FAN_AUTO) _fan_state = OFF;
}

void Thermostat::on_heat() {
  off_cool();
  _heat = ON;
  _fan_state = ON;
}

void Thermostat::on_cool() {
  off_heat();
  _cool = ON;
  _fan_state = ON;
}

int8_t Thermostat::selected_menu_item_math(int8_t selected_menu_item) {
  /*
  This simple if logic is used multiple times so a function was made
  */

  if (selected_menu_item > _current_menu_item_count - 1) return 0;
  else if (selected_menu_item < 0) return _current_menu_item_count - 1;
  else return selected_menu_item;
}

void Thermostat::set_backlight() {
  /*
  Set backlight color
  */

  _lcd.setBacklight(TEAL);
}

void Thermostat::set_temp(uint8_t temp) {
  /*
  Set the current temperature
  */

  _previous_temp = _temp;
  _temp = temp;
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

void Thermostat::yield() {
  /*
  Yield control to the Thermostat object
  */
  uint8_t buttons = _lcd.readButtons();

  if (_current_display == DISPLAY_HOME) {
    buttons = _lcd.readButtons();
    if (buttons) {
      if (((buttons & BUTTON_SELECT) == BUTTON_SELECT)) {
        _current_display = DISPLAY_MENU;
        _refresh = 1;
      }
      else if (buttons & BUTTON_UP) == BUTTON_UP) set_temp_setting(_temp_setting + (uint8_t) 1);
      else if (buttons & BUTTON_DOWN) == BUTTON_DOWN) set_temp_setting(_temp_setting - (uint8_t) 1);
      delay(_default_delay);
    } else {
      display_home();
    }
  }
  else if (_current_display == DISPLAY_MENU) {
    display_menu();
  }

}

void Thermostat::on_fan_relay() {
  /*
    Turn on FAN
  */
  digitalWrite(RELAY_FAN, HIGH);
}

void Thermostat::off_fan_relay() {
  /*
    Turn off Fan
  */
  digitalWrite(RELAY_FAN, LOW);
}

void Thermostat::on_cool_relay() {
  /*
    Turn on Cool
  */
  digitalWrite(RELAY_FAN, HIGH);
  digitalWrite(RELAY_COOL, HIGH);
  digitalWrite(RELAY_HEAT, LOW);
}

void Thermostat::off_cool_relay() {
  /*
    Turn off Cool
  */
  if (_fan_mode == FAN_AUTO) digitalWrite(RELAY_FAN, LOW);
  digitalWrite(RELAY_COOL, LOW);
  digitalWrite(RELAY_HEAT, LOW);
}

void Thermostat::on_heat_relay() {
  /*
    Turn on Heat
  */
  digitalWrite(RELAY_FAN, HIGH);
  digitalWrite(RELAY_COOL, LOW);
  digitalWrite(RELAY_HEAT, HIGH);
}

void Thermostat::off_heat_relay() {
  /*
    Turn off Heat
  */
  if (_fan_mode == FAN_AUTO) digitalWrite(RELAY_FAN, LOW);
  digitalWrite(RELAY_COOL, LOW);
  digitalWrite(RELAY_HEAT, LOW);
}
