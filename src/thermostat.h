#ifndef Thermostat_h
#define Thermostat_h

#include <Arduino.h>
#include <Adafruit_RGBLCDShield.h>

// LCD Colors
#define LCD_DISPLAYOFF 0x00
#define RED 0x1
#define GREEN 0x2
#define YELLOW 0x3
#define BLUE 0x4
#define VIOLET 0x5
#define TEAL 0x6
#define WHITE 0x7

#define OFF 0x00
#define ON 0x1

// Display
#define ERROR_DISPLAY 0x1
#define DISPLAY_HOME 0x2
#define DISPLAY_MENU 0x3
#define HEATER_AC_DISPLAY 0x4
#define FAN_DISPLAY 0x5

class Thermostat {
public:
  Thermostat();
  void init(uint8_t cols, uint8_t rows); // Initilize the class

  void buttons(); // Monitor buttons?
  void clear_lcd(); // Clear the LCD
  void display_home(); // Display the home screen (temperature and setting)
  void display_menu(); // Display menu options
  void get_display(); // Return what display Thermostat is on
  void lcd_blank_portion(uint8_t column, uint8_t line, uint8_t number); // Erase a portion of the LCD
  void set_backlight(); // Set the backlight color
  void set_temp(uint8_t temp); // Set the current temperature
  void set_temp_setting(uint8_t temp_setting); // Set the temperature setting
  void yield(); // Yield control the to Thermostat object

  void test();

private:
  Adafruit_RGBLCDShield _lcd;

  uint8_t _current_display; // Current display the Thermostat screen should show
  uint8_t _lcd_backlight_color; // LCD Backlight Color
  uint8_t _temp; // Current temperature
  uint8_t _temp_setting; // Temperature setting

  // HVAC Settings
  uint8_t _fan_mode; // Auto or On?
  uint8_t _fan_state; // On or Off?
  uint8_t _ac; // On or Off?
  uint8_t _heat; // On or Off?

  uint8_t _degree[8]; // Degree Symbol
  uint8_t _smiley[8]; // Smiley Symbol
  uint8_t _tongue[8]; // Smiley Symbol
};

#endif
