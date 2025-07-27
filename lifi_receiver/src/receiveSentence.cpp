#include <Arduino.h>
#include "../inc/receiveSentence.h"
#include "../inc/Bit2String.h"

void receiveSentence(int analogPin, int threshold) {
  Serial.println(F(">>> receiveSentence START"));
  Serial.print(F("Using threshold: "));
  Serial.println(threshold);

  const int BITS_PER_CHAR = 8;
  bool bits[BITS_PER_CHAR];
  String message = "";

  while (true) {
    // 1️⃣ Read 8 bits
    for (int i = 0; i < BITS_PER_CHAR; i++) {
      int value = analogRead(analogPin);
      bits[i] = (value > threshold) ? 1 : 0;

      Serial.print(F("Bit "));
      Serial.print(i);
      Serial.print(F(": "));
      Serial.print(value);
      Serial.print(F(" -> "));
      Serial.println(bits[i]);

      delayMicroseconds(30000); // match sender bit delay
    }

    // 2️⃣ Check if all bits are zero (null byte)
    bool allZero = true;
    for (int i = 0; i < BITS_PER_CHAR; i++) {
      if (bits[i]) {
        allZero = false;
        break;
      }
    }

    if (allZero) {
      Serial.println(F("Null byte detected, ignoring."));
      continue;
    }

    // 3️⃣ Convert 8 bits to string using Bit2String
    String part = Bit2String(bits, BITS_PER_CHAR);
    message += part;

    Serial.print(F("Received: "));
    Serial.println(part);
    Serial.print(F("Full message so far: "));
    Serial.println(message);
  }
}
