#ifndef Thermostat_h
#define Thermostat_h

#include <Arduino.h>
#include <Adafruit_RGBLCDShield.h>
#include <Adafruit_SleepyDog.h>

// #define DEBUG

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

// Display
#define DISPLAY_ERROR 0x1
#define DISPLAY_HOME 0x2
#define DISPLAY_MENU 0x3
#define HEATER_AC_DISPLAY 0x4
#define FAN_DISPLAY 0x5
#define DISPLAY_OVERRIDE 0x6

// Display Text
#define HEAT_TEXT "Heat"
#define COOL_TEXT "Cool"
#define OFF_TEXT "Off"
#define FAN_AUTO_TEXT "Auto"
#define FAN_ON_TEXT "On"

// Static Numeric Values
#define IDLE_TIMEOUT 15000 // If no activity turn off backlight
#define PREVIOUS_TEMP_INTERVAL 300000
#define TEMP_LOW 55
#define TEMP_HIGH 90
#define MIN_RUN_TIME_COOL 300000 // Run at least this long
#define MIN_RUN_TIME_HEAT 420000 // Run at least this long
#define MIN_STOP_TIME 360000 // Stop at least this long
#define POST_AC_FAN 30000 // Run fan for at least this
#define CURRENT_MENU_ITEM_COUNT 3 // Number of menu levels

// Relays
#define RELAY_FAN 4
#define RELAY_COOL_1 5
#define RELAY_HEAT 6
#define RELAY_COOL_2 7

class Thermostat {
public:
  Thermostat();
  void init(uint8_t cols, uint8_t rows); // Initilize the class

  Adafruit_RGBLCDShield _lcd; // LCD function

  void buttons(); // Monitor buttons?
  void clear_lcd(); // Clear the LCD
  void display_error(); // Error Message Display
  void display_home(); // Display the home screen (temperature and setting)
  void display_menu(); // Display menu options
  void get_display(); // Return what display Thermostat is on
  void set_display(uint8_t display); // Set the display
  void lcd_blank_portion(uint8_t column, uint8_t line, uint8_t number); // Erase a portion of the LCD
  void saftey_check(); // Series of saftey and sanity checks
  void set_temp(uint8_t temp); // Set the current temperature
  void set_temp_setting(uint8_t temp_setting); // Set the temperature setting
  void shutdown(); // Shutdown the system
  void yield(); // Yield control the to Thermostat object

  void auto_fan();
  void on_fan();

  void off_cool();
  void off_heat();
  void on_cool();
  void on_heat();

  void fan_relay(boolean mode);
  void cool_relay(boolean mode);
  void heat_relay(boolean mode);

  void refresh();

  void test();

  uint8_t get_set_temp();
  uint8_t get_temp();
  uint8_t get_cool();
  uint8_t get_heat();

  // HVAC Settings
  uint8_t _heat; // On or Off?
  uint8_t _heat_relay; // On or Off?
  uint8_t _cool; // On or Off?
  uint8_t _cool_relay; // On or Off?
  uint8_t _fan_mode; // Auto or On?
  uint8_t _fan_relay; // On or Off?

  uint8_t _temp; // Current temperature
  uint8_t _temp_setting; // Temperature setting
  uint8_t _temp_start; // Temp at start of cycle
  unsigned long _run_start; // When the system turned on, using system millis()
  unsigned long _run_stop; // When the system turned off, using system millis()

  uint8_t _current_display; // Current display the Thermostat screen should show
  unsigned long _idle; // How long since a button was pressed

private:
  int8_t selected_menu_item_math(int8_t selected_menu_item);

  uint8_t _previous_temp; // Previous temperature
  uint8_t _previous_temp_setting; // Previous Temperature setting

  String _error_message = ""; // Error message to display on DISPLAY_ERROR

  uint8_t _refresh; // Is this a fresh menu, e.g. refresh display

  const String current_menu_item[CURRENT_MENU_ITEM_COUNT] = {
    "Cool/Heat",
    "Fan",
    "Exit"
  };
  const String _fan_options[2] = {
    FAN_AUTO_TEXT,
    FAN_ON_TEXT
  };
  const String _hvac_options[CURRENT_MENU_ITEM_COUNT] = {
    COOL_TEXT,
    OFF_TEXT,
    HEAT_TEXT
  };
  const uint8_t selected_menu_count[CURRENT_MENU_ITEM_COUNT] = {
    3,
    2,
    1
  };

  uint8_t _default_delay;

  unsigned long _previous_temp_millis; // How long since the temperature was updated
  uint8_t _degree[8]; // Degree Symbol
  // uint8_t _smiley[8]; // Smiley Symbol
  uint8_t _right_arrow[8]; // Right pointie arrow symbol
};

#endif
