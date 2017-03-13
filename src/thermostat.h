#ifndef Thermostat_h
#define Thermostat_h

#include <Arduino.h>
#include <Adafruit_RGBLCDShield.h>

// LCD Colors
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
#define LCD_DISPLAYOFF 0x00

// HVAC Modes
#define OFF 0x1
#define HEATER 0x2
#define AC 0x4

// Fan modes
#define FAN_AUTO 0x8
#define FAN_ON 0x10

// Display
#define ERROR_DISPLAY 0x1
#define PRIMARY_DISPLAY 0x2
#define HEATER_AC_DISPLAY 0x4
#define FAN_DISPLAY 0x8

void find_temp_sensor();
void lcd_error_display();
void lcd_default_display();
void lcd_heater_ac_display();
void lcd_fan_menu();
void lcd_display_temp();
void lcd_display_temp_setting(bool);
void get_temp();
void lcd_blank_portion(uint8_t, uint8_t, uint8_t);
uint8_t lcd_default_display_menu();

class Thermostat {
public:
  Thermostat();
  void init(uint8_t cols, uint8_t rows);

  void clear_lcd();
  void display_home();
  void lcd_blank_portion(uint8_t column, uint8_t line, uint8_t number);
  void set_backlight();
  void set_temp(uint8_t temp);
  void set_temp_setting(uint8_t temp_setting);

  void test();

private:
  Adafruit_RGBLCDShield _lcd;
  uint8_t _temp; // Current temperature
  uint8_t _temp_setting; // Temperature setting
  uint8_t _lcd_backlight_color; // LCD Backlight Color

  uint8_t _degree[8]; // Degree Symbol
  uint8_t _smiley[8]; // Smiley Symbol
  uint8_t _tongue[8]; // Smiley Symbol
};

#endif
