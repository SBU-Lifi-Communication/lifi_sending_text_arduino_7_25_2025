#include "config.h"
#include "calibrate.h"
#include "prepare.h"
#include "sendcheck.h"

// Global variable definitions
const unsigned int calibrationDelay = 300; 
const char calibrationData[] = "101010101";
const int calibrationLength = sizeof(calibrationData) - 1;
const int numExpectedBytes = (calibrationLength + 7) / 8;
byte expectedBytes[numExpectedBytes];

float threshold = 512.0; 

const int outPin = 2;
const int sensorPin = A0;

const unsigned int bitTime = 9700;
const unsigned int settleTime = 9700;

int errorBytes = 0;
unsigned long totalBytes = 0;

void setup() {
  pinMode(outPin, OUTPUT);
  Serial.begin(31250);
  delay(1000);

  calibrate();
  prepareData();
}

void loop() {
  sendAndCheck();
}
