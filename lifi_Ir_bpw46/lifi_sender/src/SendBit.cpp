#include <Arduino.h>
#include "../inc/SendBit.h"

void sendBits(bool *bits, int length, unsigned int bitDelayMicros, int outPin) {
  Serial.println(">>> sendBits STARTED");
  pinMode(outPin, OUTPUT);
  Serial.print("Sent: ");

  // Schedule each bit against an absolute timeline to avoid cumulative drift.
  uint32_t start = micros();

  for (int i = 0; i < length; i++) {
    digitalWrite(outPin, bits[i] ? HIGH : LOW);
    Serial.print(bits[i]);

    // Target time for the *end* of this bit slot
    uint32_t target = start + (uint32_t)(i + 1) * bitDelayMicros;

    // Spin until we reach the scheduled boundary (overflow-safe compare)
    while ((int32_t)(micros() - target) < 0) { /* wait */ }
  }

  Serial.println();
}
