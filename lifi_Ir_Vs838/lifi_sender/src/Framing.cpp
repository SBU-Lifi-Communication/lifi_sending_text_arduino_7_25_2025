#include "../inc/Framing.h"

int addFraming(const bool *inputBits, bool *outBits) {
  int idx = 0;
  outBits[idx++] = 1;            // Start frame bit
  for (int i = 0; i < 12; i++) {
    outBits[idx++] = inputBits[i];
  }
  outBits[idx++] = 1;            // End frame bit
  return idx;                    
}