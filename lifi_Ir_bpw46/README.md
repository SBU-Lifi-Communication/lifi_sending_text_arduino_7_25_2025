# Delayless, Drift-Free Timing + Framed Hamming (UNO R4)

A drop-in timing refactor for a framed **Hamming(12,8)** link on Arduino **UNO R4 (Minima/WiFi)**.  
It replaces `delayMicroseconds(15000)` with rollover-safe, **absolute‐time scheduling** (`micros()`), preserving blocking behavior and exact pacing while eliminating long-term drift.

---

## Contents
- [Highlights](#highlights)
- [What’s in the repo](#whats-in-the-repo)
- [How the timing works](#how-the-timing-works)
- [Protocol (framing + Hamming)](#protocol-framing--hamming)
- [Configuration](#configuration)
- [Quick start](#quick-start)
- [Tuning](#tuning)
- [Troubleshooting](#troubleshooting)
- [FAQ](#faq)
- [Quick reference](#quick-reference)

---

## Highlights

- **Drift-free cadence:** Each bit is sampled on an absolute schedule (`next_due`) instead of “sleeping” roughly for a period.
- **Rollover-safe:** Signed delta compare across the ~**71 min** `micros()` wrap.
- **Zero redesign:** Keeps your **[start | 12 code | end]** framing and **Hamming(12,8)** path.
- **Cooperative wait (opt-in):** `-DWAIT_UNTIL_US_YIELD` lets the busy-wait call `yield()` on Wi-Fi/RTOS boards.

---

## What’s in the repo


**inc/**
- wait_until_us.h # Absolute wait helper (wrap-safe; optional yield)
- receiveSentence.h # Receiver declaration
- receiveCalibration.h # Receiver-side threshold calibration (analog)
- sendSentence.h # Sender API
- SendCalibration.h # Sender-side calibration routine
- 
**src/**
- receiveSentence.cpp # Fixed-cadence sampling + framing + Hamming decode
... Framing/Hamming encode/decode sources ...

> Using a different layout? Adjust include paths (e.g., `../inc/...`) or colocate headers with sources.

---

## How the timing works

1. **Set the first deadline**  
   `next_due = micros() + INTERVAL_US`
2. **Block precisely**  
   Busy-wait until `micros()` reaches `next_due`.  
   *(Optional)* `yield()` inside the wait if `WAIT_UNTIL_US_YIELD` is defined.
3. **Sample + log (optional)**
4. **Advance**  
   `next_due += INTERVAL_US` and repeat for all bits.

> **Rollover rule**  
> Use a signed compare: `(long)(micros() - target) < 0` stays correct across the wrap.

---

## Protocol (framing + Hamming)

### Frame (14 bits)
| Bit | Meaning             |
|-----|---------------------|
| 1   | Start = `1`         |
| 2–13| Hamming(12,8) code  |
| 14  | End = `1`           |

### Hamming(12,8) placement (1-indexed)
- **Parity:** 1, 2, 4, 8  
- **Data:** 3, 5, 6, 7, 9, 10, 11, 12

> Keep the same mapping on **both** sender and receiver. Mixing variants requires aligning these positions.

---

## Configuration

| Key                    | Default       | Notes                                                                 |
|------------------------|---------------|-----------------------------------------------------------------------|
| `INTERVAL_US`          | `15000` µs    | Symbol period; shorten only after removing verbose prints.            |
| `threshold`            | (calibrated)  | Analog cutoff for `(1024 - analogRead(pin)) > threshold`.             |
| `WAIT_UNTIL_US_YIELD`  | *(unset)*     | Define to enable cooperative yielding inside the wait loop.           |
| **Receiver pin**       | `A0`          | Change in your sketch if needed.                                      |
| **Sender pin**         | `2` (digital) | Configure to your hardware.                                           |
| **Baud**               | `9600`        | Set the Serial Monitor to match; “NL” or “CR/LF” both work.           |

---

## Quick start

1. **Flash the sender.** It runs `sendCalibration(outPin)` once and then waits for messages.
2. **Flash the receiver.** It prints “Waiting for calibration…”, runs `receiveCalibration(analogPin)` to derive a **threshold**, then starts `receiveSentence(analogPin, threshold)`.
3. **Send text.** Type in the sender’s Serial Monitor and press **Enter**. The receiver prints decoded characters.

---

## Tuning

- **Start conservative.** Keep `INTERVAL_US = 15000` until the link is stable; then step down gradually.
- **Silence the chatter.** Disable per-bit `Serial.print` during receive; printing dominates timing on small MCUs.
- **Buffers.** For long messages, prefer a fixed `char[]` ring buffer over `String` to avoid heap fragmentation.
- **Consistency first.** Confirm sender and receiver share the same `[1 | 12 | 1]` framing and Hamming layout.

---

## Troubleshooting

- **“Header not found”** (e.g., `inc/wait_until_us.h`)  
  Include path mismatch. Move the header or update your `#include`(s).
- **Missed frames / jitter**  
  Remove per-bit logging, keep `INTERVAL_US` generous, and avoid heavy work inside the bit loop.
- **Frequent framing errors**  
  Threshold too high/low, or the sender isn’t using the `[start | 12 | end]` contract.
- **Hamming decode issues**  
  Parity/data positions differ between sides, or encode/decode variants don’t match.

---

## FAQ

**Does it still block?**  
Yes—exactly where `delayMicroseconds()` used to. The difference is **stable, absolute** scheduling.

**How do I change the bit rate?**  
Set `INTERVAL_US`, re-tune `threshold`, and keep logging minimal before pushing lower.

**Why not use a timer ISR?**  
You can. This refactor is minimal and preserves structure. An ISR-driven sampler can be layered in later.

---

## Quick reference

- **`wait_until_us(target_us)`**  
  Busy-wait until `micros() >= target_us`. Wrap-safe; optional cooperative `yield`.
- **`receiveSentence(analogPin, threshold)`**  
  Fixed-cadence 14-bit frame sampling → deframe → Hamming(12,8) → accumulate/print.
- **Calibration**  
  `sendCalibration(outPin)` ↔ `receiveCalibration(analogPin)` to derive a stable `threshold`.

---
