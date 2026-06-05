# 03 — Firmware: LED Matrix Graphics Library

## Purpose

13×8 grayscale LED matrix graphics library for the Arduino UNO Q. Provides bat silhouettes, status indicators, and animation functions used by the classifier and integration layers.

## Dependencies

- Arduino UNO Q hardware (LED matrix built-in)
- **Output**: library files consumed by `04_firmware_classifier` and `05_firmware_integration`

## Files to Create

| File | Role |
|------|------|
| `src/cavitysense_display.h` | Display API: `show_idle()`, `show_bat()`, `show_wind()`, `show_startup_graph()` |
| `src/cavitysense_display.cpp` | Frame buffer definitions, animation logic, brightness scaling |
| `src/icon_data.h` | All icon frame data as `uint8_t[104]` arrays |
| `examples/led_test/led_test.ino` | Standalone test: cycle through all icons and animations |
| `examples/led_test/led_test.yaml` | App Lab sketch metadata |

## Key Hardware Facts

- Resolution: **13 columns × 8 rows** = 104 pixels
- Grayscale: **8 levels** (0 = off, 7 = max brightness)
- Buffer: flat `uint8_t[104]`, row-major
- Index: `index = row * 13 + column`
- Draw API: `matrix.draw(frame)` (NOT `loadFrame`)
- Layout convention: icons in cols 0-9, status bar in cols 10-12

## Step-by-Step Implementation

### 1. Icon Data (`icon_data.h`)

Define all frame buffers as `static const uint8_t NAME[104]`.

#### Bat Silhouette (cols 0-9, centered)

```cpp
// ICON_BAT — wildlife activity detected (cols 0-9, 3-bit grayscale 0-7)
// Rows 0-7, Columns 0-12 (cols 10-12 = 0, status bar overwrites them)
static const uint8_t ICON_BAT[104] = {
  // Row 0
  0,0,0,1,0,0,0,0,1,0, 0,0,0,
  // Row 1
  0,0,2,3,2,0,0,2,3,2, 0,0,0,
  // Row 2
  0,3,4,5,4,3,0,3,4,5, 0,0,0,
  // Row 3
  0,4,6,7,7,6,5,6,7,7, 0,0,0,
  // Row 4
  0,4,6,7,7,7,7,7,7,7, 0,0,0,
  // Row 5
  0,0,4,6,7,7,7,7,7,6, 0,0,0,
  // Row 6
  0,0,0,3,5,7,7,5,3,0, 0,0,0,
  // Row 7
  0,0,0,0,3,5,0,3,0,0, 0,0,0,
};
```

#### IDLE — monitoring / no activity (hollow ring)

```cpp
static const uint8_t ICON_IDLE[104] = {
  0,0,0,0,0,0,0,0,0,0, 0,0,0,
  0,0,0,7,7,7,7,7,0,0, 0,0,0,
  0,0,7,0,0,0,0,0,7,0, 0,0,0,
  0,0,7,0,0,0,0,0,7,0, 0,0,0,
  0,0,7,0,0,0,0,0,7,0, 0,0,0,
  0,0,7,0,0,0,0,0,7,0, 0,0,0,
  0,0,0,7,7,7,7,7,0,0, 0,0,0,
  0,0,0,0,0,0,0,0,0,0, 0,0,0,
};
```

#### WIND — wind/noise detected (wavy lines)

```cpp
static const uint8_t ICON_WIND[104] = {
  0,0,0,0,0,0,0,0,0,0, 0,0,0,
  0,0,0,0,1,2,2,1,0,0, 0,0,0,
  0,0,1,2,4,5,3,2,1,0, 0,0,0,
  0,0,0,0,2,5,5,2,0,0, 0,0,0,
  0,0,0,0,1,3,3,1,0,0, 0,0,0,
  0,0,0,1,3,5,4,2,1,0, 0,0,0,
  0,0,0,0,1,2,2,1,0,0, 0,0,0,
  0,0,0,0,0,0,0,0,0,0, 0,0,0,
};
```

#### TREE — tree cavity monitoring

```cpp
static const uint8_t ICON_TREE[104] = {
  0,0,0,0,0,3,3,0,0,0, 0,0,0,
  0,0,0,0,3,5,5,3,0,0, 0,0,0,
  0,0,0,0,3,5,5,3,0,0, 0,0,0,
  0,0,0,0,3,5,5,3,0,0, 0,0,0,
  0,0,0,0,3,7,7,3,0,0, 0,0,0,  // cavity indicated
  0,0,0,4,6,7,7,6,4,0, 0,0,0,
  0,0,0,4,6,7,7,6,4,0, 0,0,0,
  0,0,0,0,4,4,4,4,0,0, 0,0,0,
};
```

### 2. Status Bar (cols 10-12)

Col 10: dim separator (always brightness 1)
Cols 11-12: heartbeat indicator (pulses brightness 2↔5)

### 3. `cavitysense_display.h` API

```cpp
#ifndef CAVITYSENSE_DISPLAY_H
#define CAVITYSENSE_DISPLAY_H

#include <Arduino.h>
#include <Arduino_LED_Matrix.h>

void display_init();
void display_idle();
void display_bat();
void display_wind();
void display_tree();
void display_startup_graph(unsigned long now_ms);
void display_clear();
void display_set_heartbeat(bool on);

// Overlay status bar on cols 10-12 of any icon frame
void display_with_status(const uint8_t icon[104]);

#endif
```

### 4. Animation — Flapping Bat

Two frames cycled on detection:

```cpp
static const uint8_t BAT_WINGS_UP[104] = { /* see above with wings extended */ };
static const uint8_t BAT_WINGS_DOWN[104] = { /* see above with wings down */ };

void display_bat_animate(unsigned long now_ms) {
  // Alternate every ~200ms based on current ms
  if ((now_ms / 200) % 2 == 0)
    display_with_status(BAT_WINGS_UP);
  else
    display_with_status(BAT_WINGS_DOWN);
}
```

### 5. Brightness Scaling (activity-reactive)

```cpp
void display_scale_brightness(const uint8_t src[104], uint8_t dst[104], uint8_t level) {
  // level: 0-7, derived from FFT energy
  for (int i = 0; i < 104; i++) {
    dst[i] = (src[i] * level) / 7;
  }
}
```

### 6. Test Sketch (`examples/led_test/led_test.ino`)

```cpp
#include <Arduino_LED_Matrix.h>
#include "cavitysense_display.h"

ArduinoLEDMatrix matrix;

void setup() {
  matrix.begin();
  matrix.clear();
  display_init();
}

void loop() {
  display_idle();
  delay(2000);
  display_bat();
  delay(2000);
  display_wind();
  delay(2000);
  display_tree();
  delay(2000);
  display_clear();
  delay(1000);
}
```

## Expected Outputs

- `src/cavitysense_display.h` and `src/cavitysense_display.cpp`
- `src/icon_data.h` — all icon frame buffers
- `examples/led_test/` — standalone test sketch

## Verification

- [ ] Each icon renders correctly (visually confirm on matrix)
- [ ] Flapping bat animation cycles smoothly
- [ ] Status bar heartbeat pulses on cols 11-12
- [ ] Col 10 shows dim separator line
- [ ] Brightness scaling works (0=off, 7=full)
- [ ] `display_clear()` turns off all LEDs
