#include <Arduino.h>
#include "../inc/SendBit.h"

void sendBits(bool *bits, int length, unsigned int bitDelayMicros, int outPin) {
  pinMode(outPin, OUTPUT);
  for (int i = 0; i < length; i++) {
    digitalWrite(outPin, bits[i] ? HIGH : LOW);
    delayMicroseconds(bitDelayMicros);
  }
  digitalWrite(outPin, LOW);
}
