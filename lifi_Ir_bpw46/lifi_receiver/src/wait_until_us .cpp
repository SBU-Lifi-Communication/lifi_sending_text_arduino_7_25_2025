#include <Arduino.h>
#include "../inc/wait_until_us.h"

static inline void wait_until_us(unsigned long target) {
    while (micros() < target);
}