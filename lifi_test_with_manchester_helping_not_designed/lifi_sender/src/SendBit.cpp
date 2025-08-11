#include <Arduino.h>
#include "../inc/SendBit.h"

void sendBits(bool *bits, int length, unsigned int bitDelayMicros, int outPin) {
  Serial.println(">>> sendBits STARTED");
  Serial.print("Sent: ");
  for (int i = 0; i < length; i++) {
    digitalWrite(outPin, bits[i] ? HIGH : LOW);
    Serial.print(bits[i]);
    delayMicroseconds(bitDelayMicros);
  }
  Serial.println();
}
