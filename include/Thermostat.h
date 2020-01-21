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

#define OFF 0x0
#define ON 0x1
#define FAN_AUTO 0x0
#define FAN_ON 0x1

// Display
#define ERROR_DISPLAY 0x1
#define DISPLAY_HOME 0x2
#define DISPLAY_MENU 0x3
#define HEATER_AC_DISPLAY 0x4
#define FAN_DISPLAY 0x5

// Relays
#define RELAY_FAN 5
#define RELAY_COOL 4
#define RELAY_HEAT 6

class Thermostat {
public:
  Thermostat();
  void init(uint8_t cols, uint8_t rows); // Initilize the class

  void buttons(); // Monitor buttons?
  void clear_lcd(); // Clear the LCD
  void display_home(); // Display the home screen (temperature and setting)
  void display_menu(); // Display menu options
  void fan_auto();
  void fan_on();
  void get_display(); // Return what display Thermostat is on
  void lcd_blank_portion(uint8_t column, uint8_t line, uint8_t number); // Erase a portion of the LCD
  void set_backlight(); // Set the backlight color
  void set_temp(uint8_t temp); // Set the current temperature
  void set_temp_setting(uint8_t temp_setting); // Set the temperature setting
  void yield(); // Yield control the to Thermostat object

  uint8_t get_set_temp();
  uint8_t get_temp();
  uint8_t get_cool();
  uint8_t get_heat();

  void off_cool();
  void off_heat();
  void on_cool();
  void on_heat();

  void on_fan_relay();
  void off_fan_relay();
  virtual void on_cool_relay();
  virtual void off_cool_relay();
  virtual void on_heat_relay();
  virtual void off_heat_relay();

  void test();

private:
  Adafruit_RGBLCDShield _lcd;

  int8_t selected_menu_item_math(int8_t selected_menu_item);

  uint8_t _current_display; // Current display the Thermostat screen should show
  uint8_t _lcd_backlight_color; // LCD Backlight Color
  uint8_t _previous_temp; // Previous temperature
  uint8_t _temp; // Current temperature
  uint8_t _previous_temp_setting; // Previous Temperature setting
  uint8_t _temp_setting; // Temperature setting

  uint8_t _refresh; // Is this a fresh menu, e.g. refresh display

  // HVAC Settings
  uint8_t _heat; // On or Off?
  uint8_t _cool; // On or Off?
  uint8_t _fan_mode; // Auto or On?
  uint8_t _fan_state; // On or Off?

  const String _heat_text = "Heat";
  const String _cool_text = "Cool";
  const String _off_text = "Off";
  const String _fan_auto_text = "Auto";
  const String _fan_on_text = "On";

  const static uint8_t _current_menu_item_count = 3;
  const String current_menu_item[_current_menu_item_count] = {
    "Cool/Heat",
    "Fan",
    "Exit"
  };
  const String _fan_options[2] = {
    _fan_auto_text,
    _fan_on_text
  };
  const String _hvac_options[_current_menu_item_count] = {
    _cool_text,
    _off_text,
    _heat_text
  };
  const uint8_t selected_menu_count[_current_menu_item_count] = {
    3,
    2,
    1
  };

  uint8_t _default_delay;

  uint8_t _degree[8]; // Degree Symbol
  uint8_t _smiley[8]; // Smiley Symbol
  uint8_t _right_arrow[8]; // Right pointie arrow symbol
};

#endif
