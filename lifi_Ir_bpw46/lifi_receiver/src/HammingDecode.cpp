#include "../inc/HammingDecode.h"

bool decodeHamming(const bool *input, int length, bool *output, int &outLen) {
  if (length != 12) return false;

  bool bits[12];
  memcpy(bits, input, 12);

  int p1 = bits[0] ^ bits[2] ^ bits[4] ^ bits[6] ^ bits[8] ^ bits[10];
  int p2 = bits[1] ^ bits[2] ^ bits[5] ^ bits[6] ^ bits[9] ^ bits[10];
  int p4 = bits[3] ^ bits[4] ^ bits[5] ^ bits[6] ^ bits[11];
  int p8 = bits[7] ^ bits[8] ^ bits[9] ^ bits[10] ^ bits[11];

  int errorPos = p1 + (p2 << 1) + (p4 << 2) + (p8 << 3);
  if (errorPos >= 1 && errorPos <= 12) {
    bits[errorPos - 1] = !bits[errorPos - 1];  // Correct the bit
  }

  // Remove parity bits to get 8 data bits
  int dataIndex = 0;
  for (int i = 0; i < 12; i++) {
    if (i != 0 && i != 1 && i != 3 && i != 7) {
      output[dataIndex++] = bits[i];
    }
  }

  outLen = 8;
  return true;
}
