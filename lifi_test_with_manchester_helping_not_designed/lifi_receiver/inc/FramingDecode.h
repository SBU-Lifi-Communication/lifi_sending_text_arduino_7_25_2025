#ifndef FRAMING_DECODE_H
#define FRAMING_DECODE_H

#include <Arduino.h>

bool removeFraming(const bool *input, int length, bool *output, int &outLen);

#endif
