# CavitySense — Wiring Schematic

```text
Arduino UNO Q                        ELV MEMS1 module (SPU0410LR5H-QB)
-------------                        -----------------------------------
     3.3V  -------------------------> VDD
     GND   -------------------------> GND
     A0    <------------------------- OUT     (analog audio out, AC-coupled)

Module internal components:
     OUT o--||--o SPU0410LR5H OUT   C1 = 1 µF DC-block capacitor
            C1
     VDD o--||--o GND               C2 = 1 µF decoupling capacitor
            C2
```

## Physical Header Locations (UNO Q)

```
Digital header (top row, left to right):
  D0  D1  D2  D3  D4  D5  D6  D7  D8  D9  D10  D11  D12  D13
                                                           
Analog header (bottom row, left to right):
  A0  A1  A2  A3  A4  A5
```

Used pins: A0 (analog audio in), 3.3V, GND.
