#pragma once
#include <Arduino.h>

// Logical '1' = HIGH->LOW mid-bit; Logical '0' = LOW->HIGH mid-bit (IEEE 802.3 style)
struct ManchConfig {
  uint32_t bit_us;   // total bit time in microseconds
  uint8_t  osr;      // samples per half-bit on RX (e.g., 5-9)
};

void manchSendBit(bool bit, int outPin, const ManchConfig& cfg, uint32_t& t_next);
void manchSendByte(uint8_t b, int outPin, const ManchConfig& cfg, uint32_t& t_next);
bool manchRXHalfbitLevel(int analogPin, int threshold, uint8_t osr);
bool manchDecodeBit(bool firstHalfLevel, bool secondHalfLevel, bool& outBit);
