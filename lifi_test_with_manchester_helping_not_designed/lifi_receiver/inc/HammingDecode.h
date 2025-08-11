#ifndef HAMMING_DECODE_H
#define HAMMING_DECODE_H

#include <Arduino.h>

bool decodeHamming(const bool *input, int length, bool *output, int &outLen);

#endif
