Arduino Li-Fi Communication Project
===================================

This project implements a Li-Fi (Light Fidelity) communication system using Arduino boards, a regular LED for transmitting, and a regular LDR (photocell) for receiving. It supports two modes:

- Single Arduino Mode (lifi_one_arduino) – Send and receive data on one Arduino for testing.
- Two-Arduino Mode (lifi_sender & lifi_receiver) – Dedicated sender and receiver using visible light.

Project Structure:
------------------
inc/              -> Header files
src/              -> Source code (.cpp)
lifi_one_arduino/ -> Single board test
lifi_sender/      -> Transmitter code
lifi_receiver/    -> Receiver code

Features:
---------
- String to bit conversion (String2Bit)
- Bit to string conversion (Bit2String)
- Framed transmission with 1 bits 1 for each byte
- Calibration pattern with preamble (01) and alternating 1010 bits
- Threshold detection using receiveCalibration
- Handshake peak detection before data transfer
- Serial CLI for sending messages
- Works with one or two Arduino boards

Hardware:
---------
- Arduino Uno R4 Minima (or compatible)
- Regular LED for transmission
- Regular LDR (photocell) for reception
- Resistors and wiring for the LED and LDR circuit

Wiring (basic):
---------------
- LED anode to Arduino TX pin (e.g. D2) through a current limiting resistor.
- LED cathode to GND.
- LDR in a voltage divider connected to Arduino analog input (e.g. A0).
- One leg of the LDR to 5V, the other leg to A0 and a resistor to GND.
- Choose the resistor value to get good range (typically 10k–100k).

Usage:
------
1. Clone the repository.
2. For single Arduino test, upload lifi_one_arduino sketch.
3. For two Arduino setup:
   - Upload lifi_sender to the transmitter board.
   - Upload lifi_receiver to the receiver board.
4. Open Serial Monitor on the sender, type a message and press Enter.
5. Receiver will calibrate, detect bits, and print the decoded message.

Important Functions:
--------------------
- String2Bit(const String &input, bool *bits): Converts a string to bit array.
- Bit2String(const bool *bits, int bitCount): Converts bit array to string.
- sendBits(bool *bits, int length, unsigned int bitDelayMicros, int outPin): Sends raw bits.
- sendSentence(const String &msg, int outPin): Frames each byte and sends message.
- sendCalibration(int outPin): Sends calibration preamble and pattern.
- receiveCalibration(int analogPin): Reads calibration bits and calculates threshold.
- handshake(int analogPin): Detects signal peaks for sync.
- receiveSentence(int analogPin, int threshold): Reads framed bytes and reconstructs the message.

Notes:
------
- Bit delay between sender and receiver must match (default: 30 ms).
- The sender holds the trailing '1' slightly longer for reliable end-bit detection.
- Use a dark tube or cover around the LDR to block ambient light for accurate readings.
- Adjust the voltage divider resistor for best sensitivity with your LED/LDR combination.
- Keep the LED and LDR aligned and close for stronger signal.
