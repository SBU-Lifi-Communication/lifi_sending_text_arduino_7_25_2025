const unsigned int calibrationDelay = 300; 
const char calibrationData[] = "101010101";
const int calibrationLength = sizeof(calibrationData) - 1;
const int numExpectedBytes = (calibrationLength + 7) / 8;
byte expectedBytes[numExpectedBytes];

float threshold = 512.0; 

const int outPin = 2;
const int sensorPin = A0;

const unsigned int bitTime = 9700;      // μs per bit
const unsigned int settleTime = 9700;   // μs settle time for signal

int errorBytes = 0;
unsigned long totalBytes = 0;

void setup() {
  pinMode(outPin, OUTPUT);
  Serial.begin(31250);
  delay(1000);

  Serial.println("=== Starting Slow Calibration ===");
  calibrate();
  Serial.print("Threshold calibrated: ");
  Serial.println(threshold);

  prepareData();
}

void loop() {
  sendAndCheck();
}

// ------------------------------------------------
//                    FUNCTIONS   :)
// ------------------------------------------------

void prepareData() {
  for (int i = 0; i < numExpectedBytes; i++) {
    byte b = 0;
    for (int bit = 0; bit < 8; bit++) {
      int idx = i * 8 + bit;
      if (idx < calibrationLength && calibrationData[idx] == '1') {
        b |= (1 << (7 - bit));
      }
    }
    expectedBytes[i] = b;
    Serial.print("Prepared byte: 0x");
    Serial.println(b, HEX);
  }
}

void calibrate() {
  int readings1 = 0, count1 = 0;
  int readings0 = 0, count0 = 0;

  for (int i = 0; i < calibrationLength; i++) {
    char bit = calibrationData[i];
    bool isOne = (bit == '1');
    digitalWrite(outPin, isOne ? HIGH : LOW);

    delay(calibrationDelay);
    int val = analogRead(sensorPin);
    delay(20);

    Serial.print("Bit ");
    Serial.print(bit);
    Serial.print(" → Reading: ");
    Serial.println(val);

    if (isOne) {
      readings1 += val;
      count1++;
    } else {
      readings0 += val;
      count0++;
    }
  }

  digitalWrite(outPin, LOW);

  float avg1 = count1 > 0 ? (float)readings1 / count1 : 0;
  float avg0 = count0 > 0 ? (float)readings0 / count0 : 0;
  threshold = (avg1 + avg0) / 2.0;

  Serial.print("Average for '1': ");
  Serial.print(avg1);
  Serial.print(" | Average for '0': ");
  Serial.print(avg0);
  Serial.print(" | → Threshold: ");
  Serial.println(threshold);
}

void sendAndCheck() {
  static int byteIndex = 0;
  byte toSend = expectedBytes[byteIndex];
  byte received = sendByte(toSend);

  Serial.print("Sent: 0x");
  Serial.print(toSend, HEX);
  Serial.print(" | Received: 0x");
  Serial.print(received, HEX);

  if (received == toSend) {
    Serial.println(" → PASS");
  } else {
    Serial.println(" → ERROR");
    errorBytes++;
  }

  totalBytes++;
  byteIndex = (byteIndex + 1) % numExpectedBytes;
}

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
