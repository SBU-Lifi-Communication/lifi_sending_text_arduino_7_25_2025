#pragma once

// ---- Debug switches ----
#define DEBUG_TX 1      // 1=enable TX logs
#define DEBUG_RX 1      // 1=enable RX logs
#define DEBUG_LEVEL 1   // 0=min, 1=bytes, 2=bits, 3=half-bits (heavy)

// Multiply bit time during debug so Serial prints fit inside timing
#define DEBUG_SLOWDOWN 4

// Apply slowdown only when debugging that side
inline uint32_t dbg_bit_us_tx(uint32_t bit_us) {
#if DEBUG_TX
  return bit_us * DEBUG_SLOWDOWN;
#else
  return bit_us;
#endif
}
inline uint32_t dbg_bit_us_rx(uint32_t bit_us) {
#if DEBUG_RX
  return bit_us * DEBUG_SLOWDOWN;
#else
  return bit_us;
#endif
}
