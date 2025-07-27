#include <Arduino.h>
#include "../inc/SendBit.h"

void sendBits(bool *bits, int length, unsigned int bitDelayMicros, int outPin) {
  Serial.println(">>> sendBits STARTED");
  pinMode(outPin, OUTPUT);
  for (int i = 0; i < length; i++) {
    digitalWrite(outPin, bits[i] ? HIGH : LOW);
    Serial.print("Sent: ");
    Serial.print(bits[i]);
    delayMicroseconds(bitDelayMicros);
  }
  digitalWrite(outPin, LOW);
  Serial.println("   Done");
}
