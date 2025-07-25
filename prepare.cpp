#include "prepare.h"
#include "config.h"

void prepareData() {
  for (int i = 0; i < numExpectedBytes; i++) {
    byte b = 0;
    for (int bit = 0; bit < 8; bit++) {
      int idx = i * 8 + bit;
      if (idx < calibrationLength && calibrationData[idx] == '1') {
        b |= (1 << (7 - bit));
      }
    }
    expectedBytes[i] = b;
  }
}
