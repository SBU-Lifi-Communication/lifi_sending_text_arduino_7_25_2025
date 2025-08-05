#include "../inc/FramingDecode.h"

bool removeFraming(const bool *input, int length, bool *output, int &outLen) {
  if (length < 3 || !input[0] || !input[length - 1]) {
    return false;
  }

  outLen = length - 2;
  for (int i = 0; i < outLen; i++) {
    output[i] = input[i + 1];
  }

  return true;
}
