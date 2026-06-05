# 07 — Hardware: Wiring, BOM, and Enclosure

## Purpose

Complete hardware documentation for the CavitySense node: bill of materials, wiring diagram, and enclosure design.

## Dependencies

- **None** — purely documentation
- Components must match what `02_firmware_audio` expects (INMP441, I2S pins D7-D10)

## Files to Create

| File | Role |
|------|------|
| `bom.md` | Bill of materials with part numbers and approximate costs |
| `wiring.md` | Step-by-step wiring instructions |
| `wiring_diagram.svg` | SVG wiring diagram |
| `schematic.md` | ASCII schematic for text-only viewing |
| `checklist.md` | Hardware validation checklist |
| `enclosure/` | Enclosure design notes and STL files (optional) |

## Step-by-Step Implementation

### 1. Bill of Materials (`bom.md`)

| Component | Part | Qty | Approx Cost | Notes |
|-----------|------|-----|-------------|-------|
| Arduino UNO Q | ABX00087 | 1 | €40-50 | Contest hardware |
| MEMS Microphone | INMP441 breakout | 1 | €3-5 | I2S digital output |
| Jumper wires | Female-female Dupont | 6 | €1 | For prototyping |
| Breadboard | Half-size | 1 | €3 | Prototyping |
| Capacitor | 100 nF ceramic | 1 | €0.10 | Decoupling near mic |
| USB-C cable | Data + power | 1 | €3 | PC connection |
| Battery (optional) | 5V power bank | 1 | €10 | Field deployment |
| Enclosure (optional) | 3D printed box | 1 | €2 | Weather protection |

**Total (minimal): ~€50** (excluding UNO Q if provided by contest)

### 2. Wiring Instructions (`wiring.md`)

#### Critical Pin Mapping

| INMP441 Pin | Connect to UNO Q | Notes |
|-------------|------------------|-------|
| VDD | 3.3V | Must be 3.3V, not 5V! |
| GND | GND | Common ground |
| SD (Data) | **D8** | I2S serial data into MCU |
| WS (Word Select) | **D10** | I2S LRCLK from MCU |
| SCK (Bit Clock) | **D9** | I2S BCLK from MCU |
| L/R (Channel) | **D7** | Driven LOW by firmware = left channel mono |

#### Additional Requirements

- Place **100 nF ceramic capacitor** between VDD and GND as close to the INMP441 breakout as possible
- Keep jumper wires short (< 15 cm) to reduce noise
- Power from 3.3V only (5V will damage the INMP441)

### 3. ASCII Schematic (`schematic.md`)

```text
Arduino UNO Q                                  INMP441
-------------                                  -------
  3.3V    -----------------------------------> VDD
  GND     -----------------------------------> GND
  D10     -----------------------------------> WS   (LRCLK)
  D9      -----------------------------------> SCK  (BCLK)
  D8      <----------------------------------- SD   (data out)
  D7      -----------------------------------> L/R  (LOW = left ch)

Optional decoupling:
  VDD o----||----o GND
         100 nF
  (placed near INMP441 breakout)
```

### 4. Validation Checklist (`checklist.md`)

- [ ] 3.3V supply verified: INMP441 VDD measures ~3.3V relative to GND
- [ ] Ground connected: mic GND and UNO Q GND are common
- [ ] I2S data: INMP441 SD → UNO Q D8
- [ ] I2S word-select: INMP441 WS → UNO Q D10
- [ ] I2S bit-clock: INMP441 SCK → UNO Q D9
- [ ] Channel select: INMP441 L/R → UNO Q D7 (firmware drives LOW)
- [ ] Decoupling: 100 nF between VDD and GND near mic
- [ ] No shorts: multimeter continuity check 3.3V ↔ GND
- [ ] Board powers up normally with mic attached
- [ ] No I2S init error in serial monitor
- [ ] Audio activity visible: peak changes on clap/tap
- [ ] Pin map verified against board documentation

### 5. Enclosure Design (optional)

For field deployment, design a small weather-resistant enclosure:

- Ventilation holes for microphone access
- USB-C cutout for power/data
- Mounting holes for tree attachment
- Simple 3D-printable box (PLA filament)

## Expected Outputs

- `bom.md` — complete parts list
- `wiring.md` — step-by-step wiring guide
- `schematic.md` — ASCII schematic
- `wiring_diagram.svg` — visual wiring diagram
- `checklist.md` — hardware validation checklist
- `enclosure/` — design notes (optional)

## Verification

- [ ] All pins correctly mapped
- [ ] Device powers on without shorts
- [ ] I2S init succeeds in firmware
- [ ] Audio signal visible in serial plotter
- [ ] Breadboard prototype matches schematic
- [ ] Checksum: each checklist item verified
