#include <Arduino.h>
#include "../inc/SendCalibration.h"
#include "../inc/SendBit.h"

void sendCalibration(int outPin) {
  Serial.println(F(">>> sendCalibration STARTED"));

  bool preamble[8] = {0, 0, 1, 1, 0, 0, 1, 1,};
  unsigned long preambleDelay = 1000000;

  Serial.println(F("[Preamble] Sending 0,1 pattern for ~3s..."));
  sendBits(preamble, 8, preambleDelay, outPin);
  Serial.println(F("[Preamble] Done."));

  bool calibration[25];
  for (int i = 0; i < 25; i++) {
    calibration[i] = (i % 2 != 0);
  }
//01010101
  Serial.println(F("[Calibration] Sending 24-bit 11001100 pattern with 30ms delay per bit..."));
  sendBits(calibration, 25, 30000, outPin);
  digitalWrite(outPin,0);
  Serial.println(F("[Calibration] Done."));
  Serial.println(F(">>> sendCalibration COMPLETE"));
}
