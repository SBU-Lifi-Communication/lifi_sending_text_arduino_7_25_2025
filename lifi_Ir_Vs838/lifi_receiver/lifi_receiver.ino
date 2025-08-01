#include <Arduino.h>

const int inPin = 2;
const int BIT_COUNT = 32;  // number of bits to capture for testing
const unsigned int bitDelay = 5000;  // µs per bit, adjust to your sending speed

bool receivedBits[BIT_COUNT];

void setup() {
  Serial.begin(9600);
  pinMode(inPin, INPUT);
  delay(1000);
  Serial.println("=== Digital Bit Receiver Test ===");

}
void loop() {
  // Capture bits
  for (int i = 0; i < BIT_COUNT; i++) {
    receivedBits[i] = digitalRead(inPin);
    delayMicroseconds(bitDelay);
  }

  // Print bits
  Serial.print("Received Bits: ");
  for (int i = 0; i < BIT_COUNT; i++) {
    Serial.print(receivedBits[i]);
  }
  Serial.println();
}
