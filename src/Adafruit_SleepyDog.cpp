#include "Adafruit_SleepyDog.h"

void MockWatchdog::enable(uint32_t timeout) {}
void MockWatchdog::reset() {}

MockWatchdog Watchdog;
