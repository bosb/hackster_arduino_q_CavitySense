# Hardware Validation Checklist

- [ ] 3.3V supply verified: ELV MEMS1 VDD ≈ 3.3V relative to GND
- [ ] Ground connected: mic GND and UNO Q GND share a common rail
- [ ] Audio out: ELV MEMS1 OUT → UNO Q A0
- [ ] No external components needed — onboard C1/C2 handle DC-block and decoupling
- [ ] No short circuits: multimeter continuity check 3.3V ↔ GND
- [ ] No ADC init error in serial monitor at 115200 baud
- [ ] Audio activity visible: serial plotter shows amplitude changes on clap/tap
- [ ] Wiring matches schematic exactly
- [ ] Board powers up normally with microphone attached
