# 07 — Hardware: Wiring, BOM, and Enclosure

## Purpose

Complete hardware documentation for the CavitySense node: bill of materials, wiring diagram, and enclosure design.

## Dependencies

- **None** — purely documentation
- Components must match what `02_firmware_audio` expects (ELV MEMS1, A0 analog in)

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
| MEMS Microphone | ELV MEMS1 (SPU0410LR5H-QB breakout) | 1 | €3-5 | Analog output, onboard 1µF DC-block + 1µF decouple |
| Jumper wires | Female-female Dupont | 3 | €0.50 | For prototyping |
| Breadboard | Half-size | 1 | €3 | Prototyping |
| Capacitor | (none — onboard) | – | – | Module includes C1 (1µF DC-block) + C2 (1µF decouple) |
| USB-C cable | Data + power | 1 | €3 | PC connection |
| Battery (optional) | 5V power bank | 1 | €10 | Field deployment |
| Enclosure (optional) | 3D printed box | 1 | €2 | Weather protection |

**Total (minimal): ~€50** (excluding UNO Q if provided by contest)

### 2. Wiring Instructions (`wiring.md`)

#### Critical Pin Mapping

| ELV MEMS1 Pin | Connect to UNO Q | Notes |
|---------------|------------------|-------|
| VDD | 3.3V | Power from 3.3V |
| GND | GND | Common ground |
| OUT | **A0** | Analog audio output (AC-coupled via onboard C1) |

#### Additional Requirements

- No external components needed — C1 (1µF DC-block) and C2 (1µF decouple) are onboard
- Keep jumper wires short (< 15 cm) to reduce noise
- Power from 3.3V only

### 3. ASCII Schematic (`schematic.md`)

```text
Arduino UNO Q                          ELV MEMS1 module
-------------                          -----------------
   3.3V    ---------------------------> VDD
   GND     ---------------------------> GND
   A0      <--------------------------- OUT     (analog, AC-coupled)

Module internal:
   OUT o--||--o SPU0410LR5H-OUT   C1 = 1 µF DC-block
        C1
   VDD o--||--o GND               C2 = 1 µF decouple
        C2
```

### 4. Validation Checklist (`checklist.md`)

- [ ] 3.3V supply verified: ELV MEMS1 VDD ≈ 3.3V relative to GND
- [ ] Ground connected: mic GND and UNO Q GND share a common rail
- [ ] Audio out: ELV MEMS1 OUT → UNO Q A0
- [ ] No external components needed — onboard C1/C2 handle DC-block and decoupling
- [ ] No shorts: multimeter continuity check 3.3V ↔ GND
- [ ] Board powers up normally with mic attached
- [ ] No ADC init error in serial monitor at 115200 baud
- [ ] Audio activity visible: serial plotter shows amplitude changes on clap/tap
- [ ] Wiring matches schematic exactly
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
- [ ] ADC + timer init succeeds in firmware
- [ ] Audio signal visible in serial plotter
- [ ] Breadboard prototype matches schematic
- [ ] Checksum: each checklist item verified
