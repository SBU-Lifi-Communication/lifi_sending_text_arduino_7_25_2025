#include <Arduino.h>
#include "../inc/handshake.h"

int handshake(int analogPin) {
  int prev = analogRead(analogPin);
  int max1 = 0;
  bool firstMaxFound = false;

  Serial.println(F(">>> handshake START"));

  while (true) {
    int curr = analogRead(analogPin);
    int diff = curr - prev;  // flipped direction to match your signal
    prev = curr;

    Serial.print(F("Curr: "));
    Serial.print(curr);
    Serial.print(F(" Diff: "));
    Serial.println(diff);

    if (diff > 50) {  // now triggers on large FALL (low 1 to high 0)
      if (!firstMaxFound) {
        max1 = diff;
        firstMaxFound = true;
        Serial.print(F("First max candidate: "));
        Serial.println(max1);
      } else {
        float lower = max1 * 0.9;
        float upper = max1 * 1.1;

        if (diff >= lower && diff <= upper) {
          Serial.print(F("Second max near first detected: "));
          Serial.println(diff);
          Serial.print(F(">>> handshake DONE, returning "));
          Serial.println(max1);
          return curr;  // or return a flag like 1 if needed
        } else {
          max1 = diff;
          Serial.print(F("New max candidate (replacing old): "));
          Serial.println(max1);
        }
      }
    }

    delayMicroseconds(1000000);  // 1s sample rate, adjust as needed
  }
}
