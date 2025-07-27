#include <Arduino.h>
#include "../inc/receiveSentence.h"
#include "../inc/Bit2String.h"

void receiveSentence(int analogPin, int threshold) {
  Serial.println(F(">>> receiveSentence START"));
  Serial.print(F("Using threshold: "));
  Serial.println(threshold);

  const int DATA_BITS = 8;
  bool dataBits[DATA_BITS];
  String message = "";

  while (true) {
    // 1️⃣ Wait for leading 1 to start a new byte
    int startVal = analogRead(analogPin);
    bool startBit = (startVal > threshold);
    if (!startBit) {
      delayMicroseconds(50000); // Skip if no start bit yet
      continue;
    }
    Serial.println(F("Start bit detected."));

    delayMicroseconds(50000); // move to next bit time

    // 2️⃣ Read 8 data bits
    for (int i = 0; i < DATA_BITS; i++) {
      int value = analogRead(analogPin);
      dataBits[i] = (value > threshold);
      Serial.print(F("Data bit "));
      Serial.print(i);
      Serial.print(F(" -> "));
      Serial.println(dataBits[i]);
      delayMicroseconds(50000);
    }

    // 3️⃣ Expect trailing 1
    int endVal = analogRead(analogPin);
    bool endBit = (endVal > threshold);
    if (!endBit) {
      Serial.println(F("Error: Missing end bit."));
      delayMicroseconds(50000);
    }
    Serial.println(F("End bit detected."));

    delayMicroseconds(50000); // move to next byte gap

    // 5️⃣ Convert to character(s)
    String part = Bit2String(dataBits, DATA_BITS);
    message += part;

    Serial.print(F("Received char: "));
    Serial.println(part);
    Serial.print(F("Full message: "));
    Serial.println(message);
  }
}
