#include <Arduino.h>
#include "../inc/receiveCalibration.h"
#include "../inc/handshake.h"

int receiveCalibration(int analogPin) {
  Serial.println(F(">>> receiveCalibration START"));

  // 1️⃣ Wait for handshake
  int middle_of_zero_and_one = handshake(analogPin);
  Serial.print(F("Handshake complete, max diff: "));
  Serial.println(maxVal);
  // 2️⃣ Read 25 bits with alternating 0/1 pattern
  const int BIT_COUNT = 25;
  long sum0 = 0;
  long sum1 = 0;
  int count0 = 0;
  int count1 = 0;
  bool firstiter = true;
  bool expectOne = false; // start with 0
  for (int i = 0; i < BIT_COUNT; i++) {
    int value = analogRead(analogPin);
    if(firstiter){
      //1 for lesser an 0 for bigger than the middle_of_zero_and_one
    while((value < middle_of_zero_and_one)){
            Serial.print("in while:");
            Serial.print(value);
            Serial.print(" middle_of_zero_and_one: ");
            Serial.println(int(middle_of_zero_and_one));
            value = analogRead(analogPin);
            delay(10);
    }
            Serial.print("out while:");
            Serial.print(value);
    firstiter = false;
}

    if (expectOne) {
      sum1 += value;
      count1++;
      Serial.print(F("Bit "));
      Serial.print(i);
      Serial.print(F(" (1): "));
      Serial.println(value);
    } else {
      sum0 += value;
      count0++;
      Serial.print(F("Bit "));
      Serial.print(i);
      Serial.print(F(" (0): "));
      Serial.println(value);
    }

    expectOne = !expectOne; // alternate 0/1
    delayMicroseconds(30000); // 100 ms per bit
  }

  // 3️⃣ Compute threshold
  float avg0 = (count0 == 13) ? (float)sum0 / count0 : 0;
  float avg1 = (count1 == 12) ? (float)sum1 / count1 : 0;
  int threshold = (avg0 + avg1) / 2;

  Serial.print(F("Avg0: "));
  Serial.print(avg0);
  Serial.print(F("  Avg1: "));
  Serial.print(avg1);
  Serial.print(F("  Threshold: "));
  Serial.println(threshold);

  Serial.println(F(">>> receiveCalibration DONE"));
  return threshold;
}
