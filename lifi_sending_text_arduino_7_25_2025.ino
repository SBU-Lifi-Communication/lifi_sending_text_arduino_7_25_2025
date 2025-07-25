#include "inc/config.h"
#include "inc/calibrate.h"
#include "inc/prepare.h"
#include "inc/sendcheck.h"

// Global variable definitions
const unsigned int calibrationDelay = 300; 
const char calibrationData[] = "101010101";
const int calibrationLength = sizeof(calibrationData) - 1;
const int numExpectedBytes = (calibrationLength + 7) / 8;
byte expectedBytes[numExpectedBytes];

float threshold = 512.0; 

const int outPin = 2;
const int sensorPin = A0;

const unsigned int bitTime = 8000;
const unsigned int settleTime = 8000;

int errorBytes = 0;
unsigned long totalBytes = 0;

void setup() {
  pinMode(outPin, OUTPUT);
  Serial.begin(31250);
  delay(1000);

  Serial.println("=== Serial test from setup ===");

  calibrate();
  prepareData();
}

void loop() {
  sendAndCheck();
}
