#include <Arduino.h>
#include "../inc/receiveSentence.h"
#include "../inc/Bit2String.h"
#include "../inc/HammingDecode.h"
#include "../inc/FramingDecode.h"

void receiveSentence(int analogPin, int threshold) {
  Serial.println(F(">>> receiveSentence START"));
  Serial.print(F("Using threshold: "));
  Serial.println(threshold);

  const int TOTAL_BITS = 14; // 1 start + 8 data + 4 hamming + 1 end
  bool framedBits[TOTAL_BITS];
  bool hammingBits[12];
  bool dataBits[8];
  int hammingLen = 0, dataLen = 0;

  String message = "";

  while (true) {
    int startVal = analogRead(analogPin);
    if (startVal <= threshold) {
      delayMicroseconds(60000);
      continue;
    }

    Serial.println(F("Start bit detected."));
    delayMicroseconds(60000);

    for (int i = 0; i < TOTAL_BITS; i++) {
      framedBits[i] = analogRead(analogPin) < threshold;
      delayMicroseconds(60000);
    }

    if (!removeFraming(framedBits, TOTAL_BITS, hammingBits, hammingLen)) {
      Serial.println(F("Framing error."));
      continue;
    }

    if (!decodeHamming(hammingBits, hammingLen, dataBits, dataLen)) {
      Serial.println(F("Hamming decode error."));
      continue;
    }

    String part = Bit2String(dataBits, dataLen);
    message += part;

    Serial.print(F("Received char: "));
    Serial.println(part);
    Serial.print(F("Full message: "));
    Serial.println(message);
  }
}
