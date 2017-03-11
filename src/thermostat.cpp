#include <OneWire.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
 #include <stdint.h>
#include <thermostat.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// Coded Symbols
byte degree[8] = {
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
};

byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};

OneWire ds(10);  // on pin 10 (a 4.7K resistor is necessary); this is the temperature sensor!
byte addr[8]; // Address of temperature sensor

int set_temp = 70; // Default temperature setting
int current_temp = 0.0; // Current temperature

byte display_mode; // Which display the user is seeing
byte mode; // Heat, Cool, Off
byte fan_mode; // Auto, On

String error_message; // Error message to display

uint8_t buttons; // What button was pressed

void setup(void) {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.createChar(0, degree);
  lcd.createChar(1, smiley);
  lcd.setBacklight(GREEN);

  find_temp_sensor();

  mode = HEATER;
  fan_mode = FAN_AUTO;
  display_mode = PRIMARY_DISPLAY;
}

void loop(void) {
  buttons = lcd.readButtons();

  if (display_mode & PRIMARY_DISPLAY) {
    lcd_default_display();
  }
  else if (display_mode & HEATER_AC_DISPLAY) {
    lcd_heater_ac_display();
  }
  else if (display_mode & FAN_DISPLAY) {
    lcd_fan_menu();
  }
  delay(100);
}

void set_mode(void) {
  /*
     Manage modes: heater, A/C, fan...
  */
  Serial.print("mode: ");
  Serial.println(mode);
  if (mode & HEATER && mode & AC) {
    // We should never get here
    error_message = F("HEAT & AC on!");
    mode = 0x00;
    lcd_error_display();
  }
  else if (mode & HEATER) {
    if (current_temp < set_temp) {
      lcd.setBacklight(RED);
    }
    else {
      lcd.setBacklight(GREEN);
    }
  }
  else if (mode & AC) {
    if (current_temp > set_temp) {
      lcd.setBacklight(BLUE);
    }
    else {
      lcd.setBacklight(GREEN);
    }
  }

  if (mode & FAN_ON) {
    // Turn on fan...
  }
}

void lcd_default_display(void) {
  /*
     Default display for normal operation
  */
  static byte last_mode = 0x0; // Last mode, 0x0 for first run, 0x1 after that
  static unsigned long last_measurement_time; // Last time the temperature was measured

  if (last_mode == 0x0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd_display_temp();
    lcd_display_temp_setting(true); // Will update regardless
  }
  last_mode = 0x1; // Don't keep refreshing

  // Display the temperature settings
  lcd_display_temp_setting(false); // Will only update if needed

  // Determine if its time to get the current temperature
  if (millis() > last_measurement_time) {
    last_measurement_time = millis() + 10000;
    get_temp();
  }

  set_mode(); // Set the thermostat mode, e.g. heat, A/C, fan

  // Display line 2: mode and fan
  last_mode = lcd_default_display_menu(); // If we change displays in this subroutine then we need to know to refresh this when we come back
}

void lcd_fan_menu(void) {
  /*
     Display Fan mode
  */
  static byte last_mode = 0x0; // Last mode, 0x0 for first run, 0x1 after that
  static uint8_t selected_menu_index = 0; // Menu item index, left to right starting at 0
  const uint8_t menu_column[2] = { // Menu column start locations
    0,
    7
  };
  const String menu_text[2] = { // Menu strings
    "Auto",
    "On"
  };

  if (last_mode == 0x0) {
    Serial.println("lcd_fan_menu: last_mode == 0x0");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Choose fan mode"));
    lcd.setCursor(menu_column[selected_menu_index], 1);
    lcd.write(0x7E);
    lcd.setCursor(menu_column[selected_menu_index] + menu_text[selected_menu_index].length() + 1, 1);
    lcd.write(0x7F);
  }
  last_mode = 0x1;
  Serial.println("lcd_fan_menu: last_mode == 0x1");

  lcd.setCursor(menu_column[0] + 1, 1);
  lcd.print(menu_text[0]);
  lcd.setCursor(menu_column[1] + 1, 1);
  lcd.print(menu_text[1]);

  Serial.println("lcd_fan_menu: checking buttons");
  if (buttons) {
    Serial.println("lcd_fan_menu: button pressed");
    if (buttons & BUTTON_SELECT) {
      Serial.println("lcd_fan_menu: BUTTON_SELECT");
      if (selected_menu_index == 0) {
        fan_mode = FAN_AUTO;
      }
      else if (selected_menu_index == 1) {
        fan_mode = FAN_ON;
      }
      display_mode = PRIMARY_DISPLAY;
      last_mode = 0x0;
      return;
    }
    else if (buttons & BUTTON_RIGHT) {
      lcd.setCursor(menu_column[selected_menu_index], 1);
      lcd.print(" ");
      lcd.setCursor(menu_column[selected_menu_index] + menu_text[selected_menu_index].length() + 1, 1);
      lcd.print(" ");
      selected_menu_index = (selected_menu_index < 1) ? selected_menu_index + 1 : 0;
    }
    else if (buttons & BUTTON_LEFT) {
      lcd.setCursor(menu_column[selected_menu_index], 1);
      lcd.print(" ");
      lcd.setCursor(menu_column[selected_menu_index] + menu_text[selected_menu_index].length() + 1, 1);
      lcd.print(" ");
      selected_menu_index = (selected_menu_index > 0) ? selected_menu_index - 1 : 1;
    }
    lcd.setCursor(menu_column[selected_menu_index], 1);
    lcd.write(0x7E);
    lcd.setCursor(menu_column[selected_menu_index] + menu_text[selected_menu_index].length() + 1, 1);
    lcd.write(0x7F);
  }

  return;
}


void lcd_heater_ac_display(void) {
  /*
     Display Heater or AC or Off choices
  */
  static byte last_mode = 0x0; // Last mode, 0x0 for first run, 0x1 after that
  static uint8_t selected_menu_index = 0; // Menu item index, left to right starting at 0
  const uint8_t menu_column[3] = { // Menu column start locations
    0,
    6,
    11
  };
  const String menu_text[3] = { // Menu strings
    "Heat",
    "A/C",
    "Off"
  };

  if (last_mode == 0x0) {
    Serial.println("lcd_heater_ac_display: last_mode == 0x0");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Choose mode"));
    lcd.setCursor(menu_column[selected_menu_index], 1);
    lcd.write(0x7E);
    lcd.setCursor(menu_column[selected_menu_index] + menu_text[selected_menu_index].length() + 1, 1);
    lcd.write(0x7F);
  }
  last_mode = 0x1;
  Serial.println("lcd_heater_ac_display: last_mode == 0x1");

  lcd.setCursor(menu_column[0] + 1, 1);
  lcd.print("Heat");
  lcd.setCursor(menu_column[1] + 1, 1);
  lcd.print("A/C");
  lcd.setCursor(menu_column[2] + (uint8_t) 1, 1);
  lcd.print("Off");

  Serial.println("lcd_heater_ac_display: checking buttons");
  if (buttons) {
    Serial.println("lcd_heater_ac_display: button pressed");
    if (buttons & BUTTON_SELECT) {
      Serial.println("lcd_heater_ac_display: BUTTON_SELECT");
      if (selected_menu_index == 0) {
        mode = HEATER;
      }
      else if (selected_menu_index == 1) {
        mode = AC;
      }
      else if (selected_menu_index == 2) {
        mode = OFF;
      }
      display_mode = PRIMARY_DISPLAY;
      last_mode = 0x0;
      return;
    }
    else if (buttons & BUTTON_RIGHT) {
      lcd.setCursor(menu_column[selected_menu_index], 1);
      lcd.print(" ");
      lcd.setCursor(menu_column[selected_menu_index] + menu_text[selected_menu_index].length() + 1, 1);
      lcd.print(" ");
      selected_menu_index = (selected_menu_index < 2) ? selected_menu_index + 1 : 0;
    }
    else if (buttons & BUTTON_LEFT) {
      lcd.setCursor(menu_column[selected_menu_index], 1);
      lcd.print(" ");
      lcd.setCursor(menu_column[selected_menu_index] + menu_text[selected_menu_index].length() + 1, 1);
      lcd.print(" ");
      selected_menu_index = (selected_menu_index > 0) ? selected_menu_index - 1 : 2;
    }
    lcd.setCursor(menu_column[selected_menu_index], 1);
    lcd.write(0x7E);
    lcd.setCursor(menu_column[selected_menu_index] + menu_text[selected_menu_index].length() + 1, 1);
    lcd.write(0x7F);
  }

  return;
}

byte lcd_default_display_menu(void) {
  /*
    Display menu for default display
  */
  static byte last_mode = 0x0; // Last mode, 0x0 for first run, 0x1 after that
  static uint8_t selected_menu_index = 0; // Menu item index, left to right starting at 0
  const uint8_t menu_column[2] = { // Menu column start locations
    0,
    10
  };
  const String menu_text[5] = { // Menu strings
    "Heat",
    "A/C",
    "Off",
    "On",
    "Auto"
  };
  static uint8_t current_menu_text[2] = {
    0,
    4
  };

  if (last_mode == 0x0) {
    lcd_blank_portion(0, 1, 16);
    lcd.setCursor(menu_column[0] + 1, 1);
    if (mode & HEATER) {
      lcd.print(menu_text[0]);
      current_menu_text[0] = 0;
    }
    else if (mode & AC) {
      lcd.print(menu_text[1]);
      current_menu_text[0] = 1;
    }
    else if (mode & OFF) {
      lcd.print(menu_text[2]);
      current_menu_text[0] = 2;
    }

    lcd.setCursor(menu_column[1] + 1, 1);
    if (fan_mode & FAN_ON) {
      lcd.print(menu_text[3]);
      current_menu_text[1] = 3;
    }
    else {
      lcd.print(menu_text[4]);
      current_menu_text[1] = 4;
    }
  }
  last_mode = 0x1;

  // Check if button pressed
  if (buttons) {
    if (buttons & BUTTON_SELECT) {
      if (selected_menu_index == 0) {
        display_mode = HEATER_AC_DISPLAY;
      }
      else if (selected_menu_index == 1) {
        display_mode = FAN_DISPLAY;
      }
      last_mode = 0x0;
      return 0x0;
    }
    if ((buttons & BUTTON_RIGHT) | (buttons & BUTTON_LEFT)) {
      if (buttons & BUTTON_RIGHT) {
        lcd.setCursor(menu_column[selected_menu_index], 1);
        lcd.print(" ");
        lcd.setCursor(menu_column[selected_menu_index] + menu_text[current_menu_text[selected_menu_index]].length() + 1, 1);
        lcd.print(" ");
        selected_menu_index = (selected_menu_index < 1) ? selected_menu_index + 1 : 0;
      }
      else if (buttons & BUTTON_LEFT) {
        lcd.setCursor(menu_column[selected_menu_index], 1);
        lcd.print(" ");
        lcd.setCursor(menu_column[selected_menu_index] + menu_text[current_menu_text[selected_menu_index]].length() + 1, 1);
        lcd.print(" ");
        selected_menu_index = (selected_menu_index > 0) ? selected_menu_index - 1 : 1;
      }

      lcd.setCursor(menu_column[selected_menu_index], 1);
      lcd.write(0x7E);
      lcd.setCursor(menu_column[selected_menu_index] + menu_text[current_menu_text[selected_menu_index]].length() + 1, 1);
      lcd.write(0x7F);
    }
  }
  return 0x1;
}

void lcd_blank_portion(byte column, byte line, byte number) {
  /*
    Erase number characters from line,column
  */
  lcd.setCursor(column, line);
  for (int i = 0; i < number; i++) {
    lcd.print(F(" "));
  }
  lcd.setCursor(column, line);
}

void lcd_display_temp(void) {
  /*
     Display the current Temperature and setting
  */
  // Write the current temperature
  lcd_blank_portion(0, 0, 8);
  lcd.print(F("T:"));
  lcd.print(current_temp);
  lcd.write(byte(0));
  lcd.print(F("F"));
}

void lcd_display_temp_setting(bool update_display) {
  /*
     Handle the UP/DOWN buttons setting the desired temperature
  */
  static int set_temp_local = 0; // Local copy of setting for comparison

  if (buttons & BUTTON_UP) {
    set_temp += 1;
  }
  if (buttons & BUTTON_DOWN) {
    set_temp -= 1;
  }

  if (set_temp == set_temp_local && !update_display) {
    return;
  }

  set_temp_local = set_temp;

  // Write the set temperature
  lcd_blank_portion(9, 0, 8);
  lcd.print(F("S:"));
  lcd.print(set_temp);
  lcd.write(byte(0));
  lcd.print(F("F"));
}

void lcd_display_smile(void) {
  /*
     Display a smile  :)
  */
  lcd.setCursor(15, 1);
  lcd.write(byte(1));
}

void lcd_error_display(void) {
  /*
    Print to LCD the Error Message and change display color
  */
  static String last_message;
  if (! last_message.equals(error_message)) {
    lcd.setBacklight(VIOLET);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(error_message);
    last_message = error_message;
  }
  display_mode = ERROR_DISPLAY;
}

void find_temp_sensor(void) {
  /*
     Find the tempeature sensor
  */
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    error_message = F("Did not find temperature sensor!");
    lcd_error_display();
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    error_message = F("Temperature sensor address CRC is not valid!");
    lcd_error_display();
  }

  ds.reset();
}

void get_temp(void) {
  /*
    Read the tempeature from the probe
  */
  byte data[12];
  float celsius;

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  for (byte i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // Byte 0 and 1 contain temperature data
  // Byte 2 and 3 provide access to Th and Tl registers
  // Byte 4 contains the configuration register data
  // Byte 5, 6, and 7 reserved for internal use

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];

  celsius = (float)raw / 16.0;
  Serial.println((float)raw);
  //  current_temp = celsius * 1.8 + 32.0;
  current_temp = round(celsius * 1.8 + 32.0);
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(current_temp);
  Serial.println(" fahrenheit");

  lcd_display_temp();
}
