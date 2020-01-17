#include "Serial.h"

void Serial::begin(unsigned int speed) {
  return;
}

void Serial::end() {
  return;
}

size_t Serial::write(std::string buf){
  std::cout << buf;
  return buf.length();
}
