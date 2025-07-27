#include "inc/SendCalibration.h"

const int outPin = 2;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Starting calibration sequence...");
  
  sendCalibration(outPin);

  Serial.println("Calibration sent.");
}

void loop() {}
