#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

extern const unsigned int calibrationDelay;
extern const char calibrationData[];
extern const int calibrationLength;
extern const int numExpectedBytes;
extern byte expectedBytes[];

extern float threshold;

extern const int outPin;
extern const int sensorPin;

extern const unsigned int bitTime;
extern const unsigned int settleTime;

extern int errorBytes;
extern unsigned long totalBytes;

#endif
