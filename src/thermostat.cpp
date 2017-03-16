#include <OneWire.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include "Thermostat.h"

Thermostat thermostat = Thermostat();

// OneWire ds(10);  // on pin 10 (a 4.7K resistor is necessary); this is the temperature sensor!
// byte addr[8]; // Address of temperature sensor

void setup(void) {
  Serial.begin(9600);
  thermostat.init((uint8_t) 16, (uint8_t) 2);

  pinMode(LED_BUILTIN, OUTPUT);

  thermostat.set_temp((uint8_t) 87);
  thermostat.display_home();

}

void loop(void) {
  // // turn the LED on (HIGH is the voltage level)
  // digitalWrite(LED_BUILTIN, HIGH);
  // // wait for a second
  // delay(500);
  // // turn the LED off by making the voltage LOW
  // digitalWrite(LED_BUILTIN, LOW);
  //  // wait for a second
  // delay(500);

  thermostat.yield();
}

Thermostat::Thermostat() {
  _temp_setting = 70;
  _previous_temp_setting = _temp_setting;
  _lcd_backlight_color = WHITE;
  _current_display = DISPLAY_HOME;
  _refresh = true;

  _fan_mode = OFF;
  _fan_state = OFF;
  _cool = OFF;
  _heat = OFF;

  _default_delay = 100;

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

    if (_cool == ON and _heat == ON) {
      _lcd.print(F("ERROR W/ HVAC"));
      off_heat();
      off_cool();
    }
    else if (_cool == ON) {
      _lcd.print(_cool_text);
    }
    else if (_heat == ON) {
      _lcd.print(_heat_text);
    }
    else {
      _lcd.print(_off_text);
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
    }

    set_backlight();
  }

  if (_lcd.readButtons()) {
    // Load Menu
    _lcd.setBacklight(YELLOW);
    clear_lcd();
  }
  delay(_default_delay);
}

void Thermostat::set_backlight() {
  /*
  Set backlight color
  */

  _lcd.setBacklight(TEAL);
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
  Display menu options
  */

  static int8_t selected_menu_item = 0;
  static int8_t selected_item = 0;
  const uint16_t select_delay = 2000;
  const uint16_t select_error_delay = 30000;

  if (_refresh) {
    _refresh = false;
    clear_lcd();
    _lcd.setBacklight(VIOLET);
    _lcd.print(F("Menu: "));
    _lcd.print(current_menu_item[selected_menu_item]);
    _lcd.setCursor(0, 1);
    for (uint8_t i = 0; i < selected_menu_count[selected_menu_item]; i++) {
      _lcd.print(F(" "));

      if (selected_item == i) {
        _lcd.write(byte(2));
      }
      if (selected_menu_item == 0) {
        _lcd.print(_hvac_options[i]);
      }
      else if (selected_menu_item == 1) {
        _lcd.print(_fan_options[i]);
      }
      else {
        _lcd.print(F("Exit?"));
      }
    }
  }

  uint8_t buttons = _lcd.readButtons();
  if (buttons) {
    _refresh = true;
    if (buttons & BUTTON_UP) {
      selected_menu_item += 1;
    }
    else if (buttons & BUTTON_DOWN) {
      selected_menu_item -= 1;
    }
    else if (buttons & BUTTON_SELECT) {
      _lcd.setBacklight(GREEN);
      if (selected_menu_item == 2) { // Exit item
        _current_display = DISPLAY_HOME;
        _refresh = true;
      }
      else if (selected_menu_item == 0) { // hvac options
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
        delay(select_delay);
        _current_display = DISPLAY_HOME;
      }
      else if (selected_menu_item == 1) { // fan options
        _lcd.setBacklight(GREEN);
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
        delay(select_delay);
        _current_display = DISPLAY_HOME;
      }
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

    if (selected_menu_item > _current_menu_item_count - 1) {
      selected_menu_item = 0;
    }
    else if (selected_menu_item < 0) {
      selected_menu_item = _current_menu_item_count - 1;
    }
  }
  delay(_default_delay);
}

void Thermostat::on_heat() {
  off_cool();
  _heat = ON;
}

void Thermostat::on_cool() {
  off_heat();
  _cool = ON;
}

void Thermostat::off_heat() {
  _heat = OFF;
}

void Thermostat::off_cool() {
  _cool = OFF;
}

void Thermostat::fan_auto() {
  _fan_mode = FAN_AUTO;
}

void Thermostat::fan_on() {
  _fan_mode = FAN_ON;
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

  if (_current_display == DISPLAY_HOME) {
    if (_lcd.readButtons()) {
      _current_display = DISPLAY_MENU;
      _refresh = true;
    }
    display_home();
  }
  else if (_current_display == DISPLAY_MENU) {
    display_menu();
  }

}
