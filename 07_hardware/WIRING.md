# ELV MEMS1 → Arduino UNO Q Wiring

## Critical Pin Mapping

| ELV MEMS1 pin | UNO Q pin | Notes |
|---------------|-----------|-------|
| VDD           | 3.3V      | Power from 3.3V — onboard C2 (1 µF) decouples |
| GND           | GND       | Common ground with UNO Q |
| OUT           | A0        | Analog audio output, onboard C1 (1 µF) AC-couples |

## Step-by-Step

1. Ensure UNO Q is **powered off**
2. Connect 3.3V → ELV MEMS1 VDD
3. Connect GND → ELV MEMS1 GND
4. Connect A0 → ELV MEMS1 OUT
5. No external capacitors needed — C1 (1 µF) and C2 (1 µF) are onboard the module
6. Re-check every connection before powering on

## Capture Note

The ELV MEMS1 (Knowles SPU0410LR5H-QB) outputs an analog signal centered at ~VDD/2 ≈ 1.65 V. The firmware self-calibrates the DC bias at startup and samples at 16 kHz via GPT timer (TIM6) + ADC.

## Verification

- Measure ~3.3V between VDD and GND at the mic
- No shorts between 3.3V and GND
- Serial monitor shows "audio_init" success
- Serial plotter shows amplitude changes when you clap near the mic
