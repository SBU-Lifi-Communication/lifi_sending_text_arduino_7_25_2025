#include <Arduino.h>
#include "../inc/Bit2String.h"

String Bit2String(const bool *bits, int bitCount) {
  String output = "";
  int charCount = bitCount / 8;

  for (int i = 0; i < charCount; i++) {
    char c = 0;
    for (int b = 0; b < 8; b++) {
      c |= (bits[i * 8 + b] ? 1 : 0) << (7 - b);
    }
    output += c;
  }

  return output;
}
