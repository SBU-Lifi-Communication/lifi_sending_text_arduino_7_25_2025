#include <Arduino.h>

const int inPin = 2;
const unsigned int bitDelay = 500000;  // µs per bit, adjust to your sending speed
const int BIT_COUNT = 14;             // total bits including first 0

bool receivedBits[BIT_COUNT];

void setup() {
  Serial.begin(9600);
  pinMode(inPin, INPUT);
  delay(1000);
  Serial.println("=== Digital Bit Receiver Test ===");
}

void loop() {
  Serial.println(digitalRead(inPin));
  delay(1000);
}
