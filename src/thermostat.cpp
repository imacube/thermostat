#include <OneWire.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include "Thermostat.h"

Thermostat thermostat = Thermostat();
//
// OneWire ds(10);  // on pin 10 (a 4.7K resistor is necessary); this is the temperature sensor!
// byte addr[8]; // Address of temperature sensor
// Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup(void) {
  Serial.begin(9600);
  thermostat.init((uint8_t) 16, (uint8_t) 2);
  // thermostat = Thermostat((uint8_t) 16, (uint8_t) 2);
  // lcd.begin(16, 2);
  // lcd.createChar(0, degree);
  // lcd.createChar(1, smiley);
  // lcd.setBacklight(GREEN);
  // lcd.setBacklight(0x2);

  // Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
  // lcd.begin(16, 2);
  // lcd.setBacklight(0x4);
  // thermostat.test();

  pinMode(LED_BUILTIN, OUTPUT);


  thermostat.set_temp((uint8_t) 87);
  thermostat.display_home();

}

void loop(void) {
  // buttons = lcd.readButtons();
  //
  // if (display_mode & PRIMARY_DISPLAY) {
  //   lcd_default_display();
  // }
  // else if (display_mode & HEATER_AC_DISPLAY) {
  //   lcd_heater_ac_display();
  // }
  // else if (display_mode & FAN_DISPLAY) {
  //   lcd_fan_menu();
  // }
  // delay(100);

  // delay(1000);

  // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);
  // wait for a second
  delay(500);
  // turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);
   // wait for a second
  delay(500);

}

Thermostat::Thermostat() {
  _temp_setting = 70;
  _lcd_backlight_color = WHITE;

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
}

void Thermostat::init(uint8_t cols, uint8_t rows) {
  _lcd = Adafruit_RGBLCDShield();
  _lcd.begin(cols, rows);
  _lcd.createChar(0, _degree);
  _lcd.createChar(1, _smiley);

  // clear_lcd();
}

void Thermostat::clear_lcd() {
  _lcd.clear();
  _lcd.setCursor(0, 0);
}

void Thermostat::display_home() {
  /*
  Display home screen
  */
  _lcd.print(F("Temp: "));
  _lcd.print(_temp);
  _lcd.write(byte(0));
  _lcd.print(F("F"));
  _lcd.setCursor(0, 1);
  _lcd.print(F("Setting: "));
  _lcd.print(_temp_setting);
  _lcd.write(byte(0));
  _lcd.print(F("F"));
  set_backlight();
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

void Thermostat::set_temp(uint8_t temp) {
  /*
  Set the current temperature
  */
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
