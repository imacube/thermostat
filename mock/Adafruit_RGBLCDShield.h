#ifndef Adafruit_RGBLCDShield_h
#define Adafruit_RGBLCDShield_h

#include <string>

typedef std::string String;

#define BUTTON_UP     0b00001
#define BUTTON_DOWN   0b00010
#define BUTTON_RIGHT  0b00100
#define BUTTON_LEFT   0b01000
#define BUTTON_SELECT 0b10000

class Adafruit_RGBLCDShield {
public:
  Adafruit_RGBLCDShield();
  void begin(uint8_t, uint8_t);
  void createChar(uint8_t, uint8_t[8]);
  void clear();
  void setCursor(uint8_t, uint8_t);
  void print(String);
  void print(uint8_t);
  void write(uint8_t);
  void setBacklight(uint8_t);
  uint8_t readButtons();
};

#endif
