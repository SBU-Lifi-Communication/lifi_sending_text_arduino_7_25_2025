#include <Arduino.h>
#include "../inc/Manchester.h"
#include "../inc/Debug.h"

// Busy-wait to the scheduled time (wrap-safe)
static inline void wait_until(uint32_t t_target) {
  while ((int32_t)(micros() - t_target) < 0) { /* spin */ }
}

void manchSendBit(bool bit, int outPin, const ManchConfig& cfg, uint32_t& t_next) {
  const uint32_t bit_us  = dbg_bit_us_tx(cfg.bit_us);
  const uint32_t half    = bit_us >> 1;

#if DEBUG_TX && (DEBUG_LEVEL >= 2)
  Serial.print(F("[TX bit] val="));
  Serial.print(bit);
  Serial.print(F(" t0="));
  Serial.print(t_next);
  Serial.print(F(" half="));
  Serial.println(half);
#endif

  if (bit) {
    // first half HIGH
    wait_until(t_next);
    digitalWrite(outPin, HIGH);
#if DEBUG_TX && (DEBUG_LEVEL >= 3)
    Serial.print(F("  HALF1 HIGH at ")); Serial.println(micros());
#endif
    t_next += half;

    // second half LOW
    wait_until(t_next);
    digitalWrite(outPin, LOW);
#if DEBUG_TX && (DEBUG_LEVEL >= 3)
    Serial.print(F("  HALF2 LOW  at ")); Serial.println(micros());
#endif
    t_next += half;

  } else {
    // first half LOW
    wait_until(t_next);
    digitalWrite(outPin, LOW);
#if DEBUG_TX && (DEBUG_LEVEL >= 3)
    Serial.print(F("  HALF1 LOW  at ")); Serial.println(micros());
#endif
    t_next += half;

    // second half HIGH
    wait_until(t_next);
    digitalWrite(outPin, HIGH);
#if DEBUG_TX && (DEBUG_LEVEL >= 3)
    Serial.print(F("  HALF2 HIGH at ")); Serial.println(micros());
#endif
    t_next += half;
  }
}

void manchSendByte(uint8_t b, int outPin, const ManchConfig& cfg, uint32_t& t_next) {
#if DEBUG_TX && (DEBUG_LEVEL >= 1)
  Serial.print(F("[TX byte] 0x"));
  if (b < 16) Serial.print('0');
  Serial.print(b, HEX);
  Serial.print(F(" '"));
  if (b >= 32 && b < 127) Serial.write(b); else Serial.write('.');
  Serial.println('\'');
#endif

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
    return false; // invalid (no transition) → let caller decide what to log
  }
  // HIGH->LOW = 1, LOW->HIGH = 0
  outBit = (firstHalfLevel && !secondHalfLevel);
  return true;
}
