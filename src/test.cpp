#include "test.h"

int main() {
  cout << "\nBegin Test Output\n";

  uint8_t result = 0;
  std::set<int> results;

  results.insert(test_heat_relay());
  results.insert(test_cool_relay());

  if (results.count(1) != 0) {
      result = 1;
  }

  if (result == 0) {
    cout << "\nSUCCESS: no errors.\n";
  }

  cout << "\nEnd Test Output\n\n";
  return result;
}
