#ifndef FRAMING_H
#define FRAMING_H

#include <Arduino.h>

// Adds a 1-bit at start and end to make a frame
// inputBits: array of input bits (12 from Hamming)
// outBits: output array (14 bits total)
int addFraming(const bool *inputBits, bool *outBits);

#endif
