#include "inc/SendCalibration.h"
#include "inc/sendSentence.h"

const int outPin = 2;

void setup() {
  Serial.begin(9600);
  while (!Serial);
}

void loop() {
  static String userMsg = "";

  // Check if user typed something in Serial Monitor
  if (Serial.available()) {
    char c = Serial.read();

    // End of line → send message
    if (c == '\n' || c == '\r') {
      if (userMsg.length() > 0) {
        Serial.print("Sending message: ");
        Serial.println(userMsg);

        sendSentence(userMsg, outPin);

        Serial.println("Message sent.");
        Serial.println("Type another message:");
        userMsg = ""; // reset for next message
      }
    } else {
      userMsg += c; // accumulate characters
    }
  }
}
