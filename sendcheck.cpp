#include <Arduino.h>
#include "sendcheck.h"
#include "sendbyte.h"
#include "config.h"

void sendAndCheck() {
  static int byteIndex = 0;
  byte toSend = expectedBytes[byteIndex];
  byte received = sendByte(toSend);

  if (received != toSend) errorBytes++;
  totalBytes++;
  byteIndex = (byteIndex + 1) % numExpectedBytes;
}
