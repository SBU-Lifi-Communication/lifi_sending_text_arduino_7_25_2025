#include "inc/String2Bit.h"
#include "inc/SendBit.h"

const int outPin = 2;
bool bitArray[300];

void setup() {
  Serial.begin(9600);
  delay(5000);
  String msg = "Hi";
  int bitCount = String2Bit(msg, bitArray);
  sendBits(bitArray, bitCount, 100000, outPin); 
}

void loop() {}
