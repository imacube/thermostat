#ifndef MockSerial_h
#define MockSerial_h

#include <cstdint>
#include <iostream>
#include <string>

class MockSerial {
public:
  void begin(unsigned int);
  void end();
  size_t write(std::string);
};

extern MockSerial Serial;

#endif
