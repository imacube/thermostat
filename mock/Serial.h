#include <cstdint>
#include <iostream>
#include <string>

class Serial {
public:
  void begin(unsigned int);
  void end();
  size_t write(std::string);
};
