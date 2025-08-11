// inc/wait_until_us.h
#pragma once
#include <Arduino.h>

/**
 * Busy-waits until micros() reaches 'target'.
 * Rollover-safe; optional yield when WAIT_UNTIL_US_YIELD is defined.
 */
static inline void wait_until_us(unsigned long target) {
  while ((long)(micros() - target) < 0) {
#ifdef WAIT_UNTIL_US_YIELD
    yield(); // why: cooperative on WiFi/RTOS boards
#endif
  }
}
