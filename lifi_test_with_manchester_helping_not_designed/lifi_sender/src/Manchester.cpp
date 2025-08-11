#include <Arduino.h>
#include "../inc/Manchester.h"

// Busy-wait to the scheduled time
static inline void wait_until(uint32_t t_target) {
  while ((int32_t)(micros() - t_target) < 0) { /* spin */ }
}

void manchSendBit(bool bit, int outPin, const ManchConfig& cfg, uint32_t& t_next) {
  const uint32_t half = cfg.bit_us >> 1;

  // 1 = HIGH->LOW mid-bit; 0 = LOW->HIGH mid-bit
  if (bit) {
    // first half HIGH
    wait_until(t_next);
    digitalWrite(outPin, HIGH);
    t_next += half;

    // second half LOW
    wait_until(t_next);
    digitalWrite(outPin, LOW);
    t_next += half;
  } else {
    // first half LOW
    wait_until(t_next);
    digitalWrite(outPin, LOW);
    t_next += half;

    // second half HIGH
    wait_until(t_next);
    digitalWrite(outPin, HIGH);
    t_next += half;
  }
}

void manchSendByte(uint8_t b, int outPin, const ManchConfig& cfg, uint32_t& t_next) {
  for (int i = 7; i >= 0; --i) {
    manchSendBit((b >> i) & 0x01, outPin, cfg, t_next);
  }
}

// Returns majority level (true=bright/high, false=dark/low) in this half-bit
bool manchRXHalfbitLevel(int analogPin, int threshold, uint8_t osr) {
  uint8_t votesHigh = 0;
  for (uint8_t i = 0; i < osr; i++) {
    int v = 1024 - analogRead(analogPin);
    if (v > threshold) votesHigh++;
  }
  return votesHigh * 2 >= osr;
}

// Decode by checking that two half-bit levels differ and mapping the direction
bool manchDecodeBit(bool firstHalfLevel, bool secondHalfLevel, bool& outBit) {
  if (firstHalfLevel == secondHalfLevel) {
    return false; // invalid (no transition) → sync lost
  }
  // HIGH->LOW = 1, LOW->HIGH = 0
  outBit = (firstHalfLevel && !secondHalfLevel) ? true : false;
  return true;
}
