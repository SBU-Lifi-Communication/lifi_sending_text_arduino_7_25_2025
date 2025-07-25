#include <Arduino.h>
#include "../inc/sendbyte.h"
#include "../inc/config.h"

byte sendByte(byte data) {
  byte received = 0;

  for (int bit = 7; bit >= 0; bit--) {
    bool bitVal = (data & (1 << bit)) != 0;

    digitalWrite(outPin, bitVal ? HIGH : LOW);
    delayMicroseconds(settleTime);

    int sensorVal = analogRead(sensorPin);
    bool receivedBit = sensorVal > threshold;

    Serial.print("Sent bit: ");
    Serial.print(bitVal);
    Serial.print(" | Sensor: ");
    Serial.print(sensorVal);
    Serial.print(" → ");
    Serial.println(receivedBit);

    received = (received << 1) | (receivedBit ? 1 : 0);
    delayMicroseconds(bitTime - settleTime);
  }

  digitalWrite(outPin, LOW);
  return received;
}
