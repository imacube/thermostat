#ifndef Arduino_h
#define Arduino_h

#include <string>

// typedef unsigned char uint8_t;
// typedef char int8_t;
// typedef unsigned char boolean;

#define mock

#define LED_BUILTIN 13
#define OUTPUT 0b1
#define HIGH 0b1
#define LOW 0b0

typedef std::string String;

unsigned long millis();
void pinMode(uint8_t, uint8_t);
String F(String);
void delay(uint32_t);
uint8_t byte(uint8_t);
void digitalWrite(uint8_t, uint8_t);

#endif
