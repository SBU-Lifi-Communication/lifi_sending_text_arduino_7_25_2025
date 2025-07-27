#include <Arduino.h>
#include "../inc/sendSentence.h"
#include "../inc/String2Bit.h"
#include "../inc/SendBit.h"

void sendSentence(const String &msg, int outPin) {
  Serial.println(F(">>> sendSentence START"));

  const int BIT_LEN = 400;
  bool bitArray[BIT_LEN];

  // Convert the message to bits
  int bitCount = String2Bit(msg, bitArray);

  Serial.print(F("Converted message to "));
  Serial.print(bitCount);
  Serial.println(F(" bits."));

  // Pad with zeros up to 400 bits
  for (int i = bitCount; i < BIT_LEN; i++) {
    bitArray[i] = 0;
  }

  Serial.print(F("Sending 400 bits on pin "));
  Serial.println(outPin);

  // Send with 30 ms per bit (30000 µs)
  sendBits(bitArray, bitCount, 30000, outPin);

  Serial.println(F(">>> sendSentence DONE"));
}
