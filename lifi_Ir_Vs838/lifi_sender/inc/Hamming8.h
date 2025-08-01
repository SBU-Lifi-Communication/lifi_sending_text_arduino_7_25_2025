#ifndef HAMMING8_H
#define HAMMING8_H

#include <Arduino.h>

// Encode 8 data bits into 12-bit Hamming(12,8)
void encodeHamming8(const bool dataBits[8], bool outBits[12]);

bool decodeHamming8(bool codeword[12], bool dataBits[8]);

#endif
