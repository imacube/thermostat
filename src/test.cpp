#include "test.h"

int main() {
  cout << "\nBegin Test Output\n\n";
  uint8_t result = 0;
  result = test_heat_relay();

  cout << "\nEnd Test Output\n";
  return result;
}
