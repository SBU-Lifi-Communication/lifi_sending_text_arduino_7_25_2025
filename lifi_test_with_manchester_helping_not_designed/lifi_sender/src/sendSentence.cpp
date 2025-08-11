#include <Arduino.h>
#include "../inc/sendSentence.h"
#include "../inc/Manchester.h"
#include "../inc/CRC8.h"
#include "../inc/Debug.h"

static const uint8_t PREAMBLE_BYTE = 0x55; // 01010101
static const uint8_t SYNC1 = 0xD5;
static const uint8_t SYNC2 = 0xAA;

// Your base config; slowdown is applied at runtime when DEBUG_TX=1
static ManchConfig cfg = {
  .bit_us = 12000,
  .osr    = 5
};

void sendSentence(const String &msg, int outPin) {
  pinMode(outPin, OUTPUT);
  digitalWrite(outPin, LOW);

  const uint8_t* data = (const uint8_t*)msg.c_str();
  uint8_t len = (uint8_t)min((size_t)255, msg.length());
  uint8_t crc = crc8_ccitt(data, len);

#if DEBUG_TX
  Serial.println(F("\n=== TX FRAME START ==="));
  Serial.print(F("bit_us=")); Serial.print(dbg_bit_us_tx(cfg.bit_us));
  Serial.print(F(" osr=")); Serial.println(cfg.osr);
  Serial.print(F("LEN=")); Serial.print(len);
  Serial.print(F(" CRC=0x")); if (crc<16) Serial.print('0'); Serial.println(crc, HEX);
  Serial.print(F("MSG=\"")); Serial.print(msg); Serial.println(F("\""));
#endif

  uint32_t t_next = micros() + 2000; // lead-in

  // Preamble
#if DEBUG_TX
  Serial.println(F("[TX] PREAMBLE x8 (0x55)"));
#endif
  for (int i = 0; i < 8; i++) manchSendByte(PREAMBLE_BYTE, outPin, cfg, t_next);

  // Sync
#if DEBUG_TX
  Serial.println(F("[TX] SYNC 0xD5 0xAA"));
#endif
  manchSendByte(SYNC1, outPin, cfg, t_next);
  manchSendByte(SYNC2, outPin, cfg, t_next);

  // Length
#if DEBUG_TX
  Serial.print(F("[TX] LEN  0x"));
  if (len < 16) Serial.print('0'); Serial.println(len, HEX);
#endif
  manchSendByte(len, outPin, cfg, t_next);

  // Payload
#if DEBUG_TX
  Serial.println(F("[TX] PAYLOAD:"));
#endif
  for (uint8_t i = 0; i < len; i++) {
#if DEBUG_TX && (DEBUG_LEVEL >= 1)
    Serial.print(F("  idx=")); Serial.print(i);
    Serial.print(F(" byte=0x")); if (data[i]<16) Serial.print('0'); Serial.print(data[i], HEX);
    Serial.print(F(" '"));
    if (data[i] >= 32 && data[i] < 127) Serial.write(data[i]); else Serial.write('.');
    Serial.println('\'');
#endif
    manchSendByte(data[i], outPin, cfg, t_next);
  }

  // CRC
#if DEBUG_TX
  Serial.print(F("[TX] CRC   0x"));
  if (crc < 16) Serial.print('0'); Serial.println(crc, HEX);
#endif
  manchSendByte(crc, outPin, cfg, t_next);

  digitalWrite(outPin, LOW);

#if DEBUG_TX
  Serial.println(F("=== TX FRAME END ===\n"));
#endif
}
