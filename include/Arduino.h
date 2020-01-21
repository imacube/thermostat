#ifndef Arduino_h
#define Arduino_h

#include <string>
#include <MockSerial.h>

#define mock

#define LED_BUILTIN 13
#define OUTPUT 0b1
#define HIGH 0b1
#define LOW 0b0

typedef std::string String;
typedef uint8_t boolean;

String F(String);
uint8_t byte(uint8_t);
void delay(uint32_t);
void digitalWrite(uint8_t, uint8_t);
unsigned long millis();
void pinMode(uint8_t, uint8_t);

#endif
