#include "../inc/Hamming8.h"

void encodeHamming8(const bool dataBits[8], bool outBits[12]) {
  bool code[13] = {0};

  code[3]  = dataBits[0];
  code[5]  = dataBits[1];
  code[6]  = dataBits[2];
  code[7]  = dataBits[3];
  code[9]  = dataBits[4];
  code[10] = dataBits[5];
  code[11] = dataBits[6];
  code[12] = dataBits[7];

  code[1] = code[3] ^ code[5] ^ code[7] ^ code[9] ^ code[11];
  code[2] = code[3] ^ code[6] ^ code[7] ^ code[10] ^ code[11];
  code[4] = code[5] ^ code[6] ^ code[7] ^ code[12];
  code[8] = code[9] ^ code[10] ^ code[11] ^ code[12];

  for (int i = 0; i < 12; i++) outBits[i] = code[i + 1];
}

bool decodeHamming8(bool codeword[12], bool dataBits[8]) {
  bool code[13];
  for (int i = 0; i < 12; i++) code[i + 1] = codeword[i];

  int p1 = code[1] ^ code[3] ^ code[5] ^ code[7] ^ code[9] ^ code[11];
  int p2 = code[2] ^ code[3] ^ code[6] ^ code[7] ^ code[10] ^ code[11];
  int p4 = code[4] ^ code[5] ^ code[6] ^ code[7] ^ code[12];
  int p8 = code[8] ^ code[9] ^ code[10] ^ code[11] ^ code[12];

  int errorPos = p1 * 1 + p2 * 2 + p4 * 4 + p8 * 8;
  if (errorPos != 0 && errorPos <= 12) {
    code[errorPos] = !code[errorPos]; // Correct error
  }

  dataBits[0] = code[3];
  dataBits[1] = code[5];
  dataBits[2] = code[6];
  dataBits[3] = code[7];
  dataBits[4] = code[9];
  dataBits[5] = code[10];
  dataBits[6] = code[11];
  dataBits[7] = code[12];

  return (errorPos != 0);
}
