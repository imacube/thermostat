#include "MockSerial.h"

void MockSerial::begin(unsigned int speed) {
  return;
}

void MockSerial::end() {
  return;
}

size_t MockSerial::write(std::string buf){
  std::cout << buf;
  return buf.length();
}

MockSerial Serial;
