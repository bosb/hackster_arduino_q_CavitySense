# CavitySense — Wiring Schematic

```text
Arduino UNO Q                           INMP441 breakout
-------------                           -----------------
     3.3V  ---------------------------> VDD
     GND   ---------------------------> GND
     D10   ---------------------------> WS  (word select / LRCLK)
     D9    ---------------------------> SCK (bit clock / BCLK)
     D8    <--------------------------- SD  (serial data out)
     D7    ---------------------------> L/R (LOW = left ch)

Optional decoupling (highly recommended):

     VDD o--||--o GND
           100nF
     Place as close to INMP441 breakout as possible
```

## Physical Header Locations (UNO Q)

```
Digital header (top row, left to right):
  D0  D1  D2  D3  D4  D5  D6  D7  D8  D9  D10  D11  D12  D13
                                                          
Power header:
  3.3V  VIN  GND  GND  AREF  A0  A1  A2  A3  A4  A5
```

Used pins: D7 (L/R control), D8 (SD data in), D9 (SCK clock), D10 (WS word select), 3.3V, GND.
