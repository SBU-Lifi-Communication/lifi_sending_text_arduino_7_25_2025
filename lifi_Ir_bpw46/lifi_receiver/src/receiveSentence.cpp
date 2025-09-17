// src/receiveSentence.cpp
#include <Arduino.h>
#include "../inc/wait_until_us.h"
#include "../inc/receiveSentence.h"
#include "../inc/Bit2String.h"
#include "../inc/HammingDecode.h"
#include "../inc/FramingDecode.h"

constexpr unsigned long INTERVAL_US = 15000;
constexpr int TOTAL_BITS = 14; // 1 start + 8 data + 4 hamming + 1 end

void receiveSentence(int analogPin, int threshold) {
  Serial.println(F(">>> receiveSentence START"));
  Serial.print(F("Using threshold: "));
  Serial.println(threshold);

  bool framedBits[TOTAL_BITS];
  bool hammingBits[12];
  bool dataBits[8];
  int hammingLen = 0, dataLen = 0;
  String message;
  //fix the next due wait time with timer based delay without drift
  while (true) {
    const int startVal = 1024 - analogRead(analogPin);
    unsigned long next_due = micros() + INTERVAL_US;

    if (startVal <= threshold) {
      wait_until_us(next_due);                 // preserve original delay
      if (!message.isEmpty()) {
        Serial.println(message);
      }
      message = "";
      continue;                                // not return: stay in receive loop
    }

    wait_until_us(next_due);                   // initial pacing before start bit
    framedBits[0] = 1;

    for (int i = 1; i < TOTAL_BITS; i++) {
      framedBits[i] = (1024 - analogRead(analogPin)) > threshold;
      Serial.print(framedBits[i]);             // prints 0/1
      next_due += INTERVAL_US;                 // fixed cadence (no drift)
      wait_until_us(next_due);
    }
    Serial.println();

    if (!removeFraming(framedBits, TOTAL_BITS, hammingBits, hammingLen)) {
      Serial.println(F("framing decode error."));
      continue;
    }

    if (!decodeHamming(hammingBits, hammingLen, dataBits, dataLen)) {
      Serial.println(F("Hamming decode error."));
      continue;
    }

    const String part = Bit2String(dataBits, dataLen);
    message += part;
    Serial.print(F("Received char: "));
    Serial.println(part);
  }
}
