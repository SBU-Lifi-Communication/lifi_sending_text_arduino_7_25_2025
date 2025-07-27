#include <Arduino.h>
#include "../inc/sendSentence.h"
#include "../inc/String2Bit.h"
#include "../inc/SendBit.h"

void sendSentence(const String &msg, int outPin) {
  Serial.println(F(">>> sendSentence START"));

  const int RAW_BUFFER = 400;
  bool rawBits[RAW_BUFFER];

  // Convert the message to raw bits
  int rawBitCount = String2Bit(msg, rawBits);

  Serial.print(F("Converted message to "));
  Serial.print(rawBitCount);
  Serial.println(F(" bits."));

  // Calculate how many bytes we have (8 bits per byte)
  int byteCount = rawBitCount / 8;

  // Output buffer with framing 1bits1
  bool framedBits[RAW_BUFFER];  // buffer big enough for framed bits
  int framedIndex = 0;

  for (int b = 0; b < byteCount; b++) {
    // Leading 1
    framedBits[framedIndex++] = 1;

    // 8 data bits
    for (int i = 0; i < 8; i++) {
      framedBits[framedIndex++] = rawBits[b * 8 + i];
    }

    // Trailing 1
    framedBits[framedIndex++] = 1;
  }

  Serial.print(F("Sending "));
  Serial.print(framedIndex);
  Serial.println(F(" bits with framing on pin "));
  Serial.println(outPin);

  // Send only the bits we built, no padding
  sendBits(framedBits, framedIndex, 60000, outPin);

  Serial.println(F(">>> sendSentence DONE"));
}
