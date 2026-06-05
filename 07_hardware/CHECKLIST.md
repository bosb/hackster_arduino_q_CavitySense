# Hardware Validation Checklist

- [ ] 3.3V supply verified: INMP441 VDD ≈ 3.3V relative to GND
- [ ] Ground connected: mic GND and UNO Q GND share a common rail
- [ ] I2S data: INMP441 SD → UNO Q D8
- [ ] I2S word-select: INMP441 WS → UNO Q D10
- [ ] I2S bit-clock: INMP441 SCK → UNO Q D9
- [ ] Channel select: INMP441 L/R → UNO Q D7 (firmware drives LOW)
- [ ] Decoupling: 100 nF between VDD and GND near mic breakout
- [ ] No short circuits: multimeter continuity check 3.3V ↔ GND
- [ ] No I2S init error in serial monitor at 115200 baud
- [ ] Audio activity visible: serial plotter shows peak changes on clap/tap
- [ ] Wiring matches schematic exactly
- [ ] Board powers up normally with microphone attached
