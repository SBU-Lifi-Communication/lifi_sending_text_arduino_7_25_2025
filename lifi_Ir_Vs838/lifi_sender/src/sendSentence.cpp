#include <Arduino.h>
#include "../inc/sendSentence.h"
#include "../inc/String2Bit.h"
#include "../inc/SendBit.h"
#include "../inc/Hamming8.h"
#include "../inc/Framing.h"

void sendSentence(const String &msg, int outPin) {
  Serial.println(F(">>> sendSentence START"));

  const int RAW_BUFFER = 500;
  bool rawBits[RAW_BUFFER];

  int rawBitCount = String2Bit(msg, rawBits);
  int byteCount = rawBitCount / 8;

  bool framedBits[RAW_BUFFER];
  int framedIndex = 0;

  for (int b = 0; b < byteCount; b++) {
    bool dataBits[8];
    for (int i = 0; i < 8; i++) dataBits[i] = rawBits[b * 8 + i];

    bool hammingBits[12];
    encodeHamming8(dataBits, hammingBits);

    bool finalBits[14];
    int bitCount = addFraming(hammingBits, finalBits);

    for (int i = 0; i < bitCount; i++) {
      framedBits[framedIndex++] = finalBits[i];
    }
  }

  Serial.print(F("Sending "));
  Serial.print(framedIndex);
  Serial.println(F(" bits with Hamming + Framing"));

  sendBits(framedBits, framedIndex, 60000, outPin);

  Serial.println(F(">>> sendSentence DONE"));
}
