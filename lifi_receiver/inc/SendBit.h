#ifndef SENDBIT_H
#define SENDBIT_H

#include <Arduino.h>

void sendBits(bool *bits, int length, unsigned int bitDelayMicros, int outPin);

#endif
