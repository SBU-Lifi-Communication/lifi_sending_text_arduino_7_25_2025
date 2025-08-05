#include <Arduino.h>
#include "../inc/handshake.h"

int handshake(int analogPin) {
  int prev = analogRead(analogPin);
  int max1 = 0;
  bool firstMaxFound = false;

  Serial.println(F(">>> handshake START"));

  while (true) {
    int curr = analogRead(analogPin);
    int diff = prev - curr;
    prev = curr;

    Serial.print(F("Curr: "));
    Serial.print(curr);
    Serial.print(F(" Diff: "));
    Serial.println(diff);

    if (diff > 50) {
      if (!firstMaxFound) {
        // First candidate for max
        max1 = diff;
        firstMaxFound = true;
        Serial.print(F("First max candidate: "));
        Serial.println(max1);
      } else {
        float lower = max1 * 0.9;
        float upper = max1 * 1.1;

        if (diff >= lower && diff <= upper) {
          // Found second max close to first
          Serial.print(F("Second max near first detected: "));
          Serial.println(diff);
          Serial.print(F(">>> handshake DONE, returning "));
          Serial.println(max1);
          return curr;
        } else {
          // New max too far from old → replace max1
          max1 = diff;
          Serial.print(F("New max candidate (replacing old): "));
          Serial.println(max1);
        }
      }
    }

    delayMicroseconds(1000000); // Adjust sample rate
  }
}
