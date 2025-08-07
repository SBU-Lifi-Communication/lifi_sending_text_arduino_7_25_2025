#include <Arduino.h>
#include "../inc/SendCalibration.h"
#include "../inc/SendBit.h"

void sendCalibration(int outPin) {
  Serial.println(F(">>> sendCalibration STARTED"));

  bool preamble[4] = {0, 0, 1, 1};
  unsigned long preambleDelay = 3000000;

  Serial.println(F("[Preamble] Sending 0,1 pattern for ~3s..."));
  sendBits(preamble, 4, preambleDelay, outPin);
  Serial.println(F("[Preamble] Done."));

  bool calibration[24];
  for (int i = 0; i < 24; i++) {
    calibration[i] = (i % 2 != 0);
  }

  Serial.println(F("[Calibration] Sending 24-bit 1010 pattern with 10ms delay per bit..."));
  sendBits(calibration, 24, 10000, outPin);
  Serial.println(F("[Calibration] Done."));

  Serial.println(F(">>> sendCalibration COMPLETE"));
}
