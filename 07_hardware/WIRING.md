# INMP441 → Arduino UNO Q Wiring

## Critical Pin Mapping

| INMP441 pin | UNO Q pin | Notes |
|-------------|-----------|-------|
| VDD         | 3.3V      | Power from 3.3V only — 5V destroys the mic |
| GND         | GND       | Common ground with UNO Q |
| SD          | D8        | I2S serial audio data into MCU |
| WS          | D10       | I2S word select (LRCLK) from MCU |
| SCK         | D9        | I2S bit clock (BCLK) from MCU |
| L/R         | D7        | Driven LOW by firmware = left channel mono |

## Step-by-Step

1. Ensure UNO Q is **powered off**
2. Connect 3.3V → INMP441 VDD
3. Connect GND → INMP441 GND
4. Connect D10 → INMP441 WS
5. Connect D9 → INMP441 SCK
6. Connect D8 → INMP441 SD
7. Connect D7 → INMP441 L/R
8. Solder/place a **100 nF ceramic capacitor** across INMP441 VDD and GND (as close to the breakout as possible)
9. Re-check every connection before powering on

## Mono Capture Note

The firmware drives D7 LOW to select the left channel. The INMP441 transmits 32-bit I2S frames with valid audio data in the upper 24 bits. The MCU captures mono at 16 kHz.

## Verification

- Measure ~3.3V between VDD and GND at the mic
- No shorts between 3.3V and GND
- I2S init succeeds in firmware (no error message)
- Serial plotter shows amplitude changes when you clap near the mic
