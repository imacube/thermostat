#ifndef Adafruit_SleepyDog_h
#define Adafruit_SleepyDog_h

#include <cstdint>

class MockWatchdog {
public:
  void enable(uint32_t);
  void reset();
};

extern MockWatchdog Watchdog;

#endif
