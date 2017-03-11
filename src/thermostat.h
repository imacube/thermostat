#include <inttypes.h>

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
