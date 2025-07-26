#include <Arduino.h>
#include "../inc/String2Bit.h"

int String2Bit(const String &input, bool *bits) {
  int bitCount = input.length() * 8;
  for (int i = 0; i < input.length(); i++) {
    char c = input[i];
    for (int b = 0; b < 8; b++) {
      bits[i * 8 + b] = (c >> (7 - b)) & 0x01;
    }
  }
  return bitCount;

}
