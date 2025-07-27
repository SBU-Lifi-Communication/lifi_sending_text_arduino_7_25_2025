#include "inc/receiveSentence.h"
#include "inc/receiveCalibration.h"

const int analogPin = A0;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Waiting for calibration...");

  int threshold = receiveCalibration(analogPin);

  Serial.print("Final Threshold: ");
  Serial.println(threshold);

  receiveSentence(analogPin, threshold);
}

void loop() {}
