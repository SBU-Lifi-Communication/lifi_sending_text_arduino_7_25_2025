#include <Arduino.h>
#include "../inc/sendcheck.h"
#include "../inc/sendbyte.h"
#include "../inc/config.h"

void sendAndCheck() {
  static int byteIndex = 0;
  byte toSend = expectedBytes[byteIndex];
  byte received = sendByte(toSend);

  Serial.print("Sent: 0x");
  Serial.print(toSend, HEX);
  Serial.print(" | Received: 0x");
  Serial.print(received, HEX);

  if (received != toSend) {
    Serial.println("→ ERROR");
    errorBytes++;
  } else {
    Serial.println("→ PASS");
  }

  totalBytes++;
  byteIndex = (byteIndex + 1) % numExpectedBytes;
}
