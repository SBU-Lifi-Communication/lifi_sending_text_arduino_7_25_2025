#include <Arduino.h>
#include "../inc/receiveSentence.h"
#include "../inc/Manchester.h"
#include "../inc/CRC8.h"
#include "../inc/Debug.h"

// ---------- small helper ----------
static inline void wait_until(uint32_t t_target) {
  while ((int32_t)(micros() - t_target) < 0) { /* spin */ }
}

// --------- polarity: above-threshold = 0, below-threshold = 1 ----------
static const bool RX_LEVELS_INVERTED = true;

// ---------- change-only logger ----------
struct RxLogger {
  int  lastState     = -1;
  int  lastThrBucket = -1;  // quantized threshold to reduce noise
  int  lastByte      = -1;
  int  invalidRun    = 0;   // kept for possible future summaries, but silent now
  int  lastEdgeDir   = -2;  // -2=none yet, 0=L->H, 1=H->L

  static const __FlashStringHelper* stateName(int s){
    switch (s){
      case 0: return F("ACQUIRE");
      case 1: return F("HUNT_SYNC1");
      case 2: return F("HUNT_SYNC2");
      case 3: return F("READ_LEN");
      case 4: return F("READ_PAYLOAD");
      case 5: return F("READ_CRC");
      default: return F("?");
    }
  }

  void onState(int s){
    if (s != lastState){
      Serial.print(F("[STATE] → "));
      Serial.println(stateName(s));
      lastState = s;
      invalidRun = 0;
    }
  }

  // Only print when threshold moves by >= 4 ADC counts (tweakable)
  void onThreshold(int thr){
    int bucket = thr >> 2; // /4
    if (bucket != lastThrBucket){
      lastThrBucket = bucket;
      Serial.print(F("[THR] ")); Serial.println(thr);
    }
  }

  // Print when we see a mid-bit edge, but only when the direction changes
  void onHalfEdge(bool first, bool second){
    if (first == second) return; // no mid-bit transition -> ignore
    int dir = first ? 1 : 0;     // 1: H->L, 0: L->H
    if (dir != lastEdgeDir){
      lastEdgeDir = dir;
      Serial.print(F("[EDGE] "));
      Serial.print(first ? F("H") : F("L"));
      Serial.print(F("→"));
      Serial.println(second ? F("H") : F("L"));
    }
  }

  // Silent now
  void onInvalidBit(){ invalidRun++; }
  void onValidBit(){ if (invalidRun > 0) invalidRun = 0; }

  void onByte(uint8_t b){
    if (b != lastByte){
      lastByte = b;
      Serial.print(F("[BYTE] 0x"));
      if (b < 16) Serial.print('0'); Serial.println(b, HEX);
    }
  }

  void onFrameOK(const String& msg){
    Serial.print(F("[OK] ")); Serial.println(msg);
  }
  void onFrameCRCFail(uint8_t want, uint8_t got){
    Serial.print(F("[CRC FAIL] calc=0x")); if (want<16) Serial.print('0'); Serial.print(want, HEX);
    Serial.print(F(" recv=0x")); if (got<16) Serial.print('0'); Serial.println(got, HEX);
  }
};

// ---------- constants (must match TX) ----------
static const uint8_t SYNC1 = 0xD5;
static const uint8_t SYNC2 = 0xAA;

static ManchConfig cfg = {
  .bit_us = 12000, // base; Debug.h may slow this down (RX will print effective value)
  .osr    = 5
};

// ---------- slow IIR threshold tracker ----------
static inline int trackThreshold(int prev, int sample, float alpha) {
  return (int)(alpha * sample + (1.0f - alpha) * prev);
}

// ---------- phase acquisition (confirm at Manchester rate) ----------
static void acquirePhase(int analogPin, int& threshold,
                         uint32_t half_us, uint8_t osr,
                         uint32_t& t_next,
                         RxLogger& LOG)
{
  const uint32_t step = max(half_us / 12, (uint32_t)200); // ~8–12 probes per half; >=200 µs

  // settle
  (void)analogRead(analogPin);
  delayMicroseconds(200);
  bool lastLevel = manchRXHalfbitLevel(analogPin, threshold, osr);

  uint32_t t_scan = micros();
  for (;;) {
    // probe forward in small steps until level changes
    while ((int32_t)(micros() - (t_scan += step)) < 0) { /* spin */ }
    bool nowLevel = manchRXHalfbitLevel(analogPin, threshold, osr);

    if (nowLevel != lastLevel) {
      // candidate edge at ~now; confirm another toggle ~half_us later
      uint32_t t0 = micros();

      // sample at the middle of the *next* half to confirm toggling
      uint32_t t_check = t0 + half_us;
      wait_until(t_check);

      bool nextLevel = manchRXHalfbitLevel(analogPin, threshold, osr);

      if (nextLevel != nowLevel) {
        // alternating halves at our rate -> lock to center of the NEXT BIT's first half
        t_next = micros() + half_us + (half_us / 2); // *** 3*half/2: first-half center of next bit ***
        Serial.print(F("[ACQ] confirmed edge, lock t_next=")); Serial.println(t_next);
        return;
      } else {
        // probably ambient flicker; keep scanning
        lastLevel = nowLevel;
        continue;
      }
    }
    lastLevel = nowLevel;
  }
}

// ---------- main receiver ----------
void receiveSentence(int analogPin, int threshold_init) {
  pinMode(analogPin, INPUT);

  RxLogger LOG;

  const uint32_t bit_us  = dbg_bit_us_rx(cfg.bit_us);
  const uint32_t half_us = bit_us / 2;

  Serial.println(F("\n=== RX START (Manchester) ==="));
  Serial.print(F("bit_us=")); Serial.print(bit_us);
  Serial.print(F(" osr=")); Serial.print(cfg.osr);
  Serial.print(F(" thr_init=")); Serial.println(threshold_init);

  int threshold = threshold_init;

  // State must be declared BEFORE the lambda so it's in scope
  enum State { ACQUIRE, HUNT_SYNC1, HUNT_SYNC2, READ_LEN, READ_PAYLOAD, READ_CRC };
  State state = ACQUIRE; LOG.onState(state);

  uint8_t byteShift = 0;
  uint8_t bitsInByte = 0;
  uint8_t expectLen = 0;
  String  msg = "";
  uint32_t t_next = 0;

  // Require some consecutive valid Manchester bits before trusting phase
  uint8_t validStreak = 0;

  // sample_half updates threshold ONLY in ACQUIRE and applies polarity inversion
  auto sample_half = [&]() -> bool {
    bool lvl = manchRXHalfbitLevel(analogPin, threshold, cfg.osr);
    if (RX_LEVELS_INVERTED) lvl = !lvl;   // above thr = 0, below thr = 1

    if (state == ACQUIRE) {               // only track while hunting
      int raw = 1024 - analogRead(analogPin);
      threshold = trackThreshold(threshold, raw, 0.005f);  // slower follow
      LOG.onThreshold(threshold);         // only logs in ACQUIRE now
    }
    return lvl;
  };

  while (true) {
    if (state == ACQUIRE) {
      acquirePhase(analogPin, threshold, half_us, cfg.osr, t_next, LOG);
      validStreak = 0;
      state = HUNT_SYNC1; LOG.onState(state);
      // first sample will be taken exactly at t_next
      continue;
    }

    // ---- FIRST HALF: sample exactly at the locked time ----
    wait_until(t_next);
    bool first = sample_half();

    // ---- SECOND HALF: one half-bit later ----
    t_next += half_us;
    wait_until(t_next);
    bool second = sample_half();

    // prepare t_next for the next bit's first half
    t_next += half_us;

    // Only log when the edge direction changes
    LOG.onHalfEdge(first, second);

    bool bitVal;
    if (!manchDecodeBit(first, second, bitVal)) {
      // Invalid Manchester bit → small phase nudge & resync hunt (silent)
      LOG.onInvalidBit();
      t_next = micros() + (half_us / 2); // nudge toward centers
      validStreak = 0;
      state = HUNT_SYNC1; LOG.onState(state);
      byteShift = 0; bitsInByte = 0;
      continue;
    }

    // polarity flips Manchester mapping → invert decoded bit
    if (RX_LEVELS_INVERTED) bitVal = !bitVal;

    LOG.onValidBit();

    // Valid Manchester bit
    if (validStreak < 255) validStreak++;

    // Shift MSB-first
    byteShift = (uint8_t)((byteShift << 1) | (bitVal ? 1 : 0));
    bitsInByte++;

    if (bitsInByte < 8) continue;

    uint8_t byte = byteShift;
    bitsInByte = 0;
    LOG.onByte(byte);

    switch (state) {
      case HUNT_SYNC1:
        if (byte == SYNC1) { state = HUNT_SYNC2; LOG.onState(state); }
        break;

      case HUNT_SYNC2:
        if (byte == SYNC2) { state = READ_LEN; LOG.onState(state); }
        else { state = ACQUIRE; LOG.onState(state); }
        break;

      case READ_LEN:
        expectLen = byte;
        msg = "";
        state = (expectLen == 0) ? READ_CRC : READ_PAYLOAD; LOG.onState(state);
        break;

      case READ_PAYLOAD:
        msg += (char)byte;
        if ((uint8_t)msg.length() >= expectLen) { state = READ_CRC; LOG.onState(state); }
        break;

      case READ_CRC: {
        uint8_t crc = crc8_ccitt((const uint8_t*)msg.c_str(), msg.length());
        if (crc == byte) LOG.onFrameOK(msg);
        else             LOG.onFrameCRCFail(crc, byte);
        state = ACQUIRE; LOG.onState(state); // optional: reacquire each frame
        break;
      }

      default: break;
    }
  }
}
