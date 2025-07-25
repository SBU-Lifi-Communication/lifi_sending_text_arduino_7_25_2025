#include "calibrate.h"
#include "config.h"

void calibrate() {
  int readings1 = 0, count1 = 0;
  int readings0 = 0, count0 = 0;

  for (int i = 0; i < calibrationLength; i++) {
    char bit = calibrationData[i];
    bool isOne = (bit == '1');
    digitalWrite(outPin, isOne ? HIGH : LOW);

    delay(calibrationDelay);
    int val = analogRead(sensorPin);
    delay(20);

    if (isOne) {
      readings1 += val;
      count1++;
    } else {
      readings0 += val;
      count0++;
    }
  }

  digitalWrite(outPin, LOW);

  float avg1 = count1 > 0 ? (float)readings1 / count1 : 0;
  float avg0 = count0 > 0 ? (float)readings0 / count0 : 0;
  threshold = (avg1 + avg0) / 2.0;
}
