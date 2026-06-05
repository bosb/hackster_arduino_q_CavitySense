# 02 — Firmware: INMP441 Audio Capture + Feature Extraction

## Purpose

I2S audio capture from INMP441 microphone at 16 kHz, 1024-sample frames, with FFT-based feature extraction. Exposes a clean API (`audio_capture.h`) used by the classifier and integration layers.

## Dependencies

- Hardware: INMP441 wired per `07_hardware/` (D10=WS, D9=SCK, D8=SD, D7=L/R)
- **Output**: library files consumed by `04_firmware_classifier` and `05_firmware_integration`

## Files to Create

| File | Role |
|------|------|
| `src/audio_capture.h` | Audio buffer API: `audio_init()`, `audio_ready()`, `audio_get_frame()`, `audio_clear_ready()` |
| `src/audio_capture.cpp` | I2S backend: pin-mapped bit-bang or DMA-based capture |
| `src/feature_extraction.h` | FFT + 8-band energy feature extraction |
| `src/feature_extraction.cpp` | ArduinoFFT wrapper, band energy computation, normalization |
| `examples/audio_test/audio_test.ino` | Standalone test sketch: capture + print peak levels |
| `examples/audio_test/audio_test.yaml` | App Lab sketch metadata (for standalone flash) |

## Step-by-Step Implementation

### 1. I2S Pin Configuration (Critical!)

The UNO Q I2S is **not** on the default Arduino I2S pins. Use:

| Function | UNO Q Pin |
|----------|-----------|
| WS (LRCLK) | **D10** |
| SCK (BCLK) | **D9** |
| SD (Data In) | **D8** |
| L/R (Channel Select) | **D7** (driven LOW = left channel mono) |

### 2. `audio_capture.h` API

```cpp
#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <Arduino.h>

#define AUDIO_SAMPLE_RATE   16000
#define AUDIO_FRAME_SAMPLES 1024   // 64 ms at 16 kHz

bool audio_init();                  // Init I2S, start capture
bool audio_ready();                 // True when a full frame is available
int16_t* audio_get_frame();        // Pointer to current 1024-sample frame
void audio_clear_ready();          // Release buffer for reuse

#endif
```

### 3. `audio_capture.cpp` — I2S Backend

Key requirements:
- Sample rate: **16 kHz** mono
- I2S frame: 32-bit (data in upper 24 bits)
- Use the correct I2S library or bit-bang for the UNO Q Zephyr core
- Buffer: double-buffered ping-pong to avoid drops
- Frame trigger: interrupt or DMA callback sets `_ready = true`

Reference: the reference project uses bit-bang I2S due to Zephyr I2S limitations. Check `arduino:zephyr:unoq` I2S availability. Bit-bang fallback:

```cpp
// Bit-bang I2S read (fallback if hardware I2S unavailable on these pins)
static void capture_sample() {
  digitalWrite(WS_PIN, LOW);  // left channel
  digitalWrite(SCK_PIN, LOW);
  delayMicroseconds(1);
  // read 32 bits from SD pin...
}
```

### 4. `feature_extraction.h/.cpp`

```cpp
#ifndef FEATURE_EXTRACTION_H
#define FEATURE_EXTRACTION_H

#include <Arduino.h>

#define NUM_FEATURE_BANDS 8

bool extract_features(const int16_t* frame, size_t frame_size,
                      float out_features[NUM_FEATURE_BANDS]);
// Returns normalized energy in 8 frequency bands

void print_features(const float features[NUM_FEATURE_BANDS]);
// Debug: print features to Serial

#endif
```

Band ranges (based on 16 kHz sample rate, FFT size 1024 → 512 bins, 0-8 kHz):

| Band | Freq Range | FFT Bins | Ecological Meaning |
|------|-----------|----------|-------------------|
| B0 | 0-500 Hz | 0-31 | Ambient rumble |
| B1 | 500-1000 Hz | 32-63 | Low noise |
| B2 | 1-2 kHz | 64-127 | Bird calls |
| B3 | 2-3 kHz | 128-191 | Mid wildlife |
| B4 | 3-4 kHz | 192-255 | Mid-high wildlife |
| B5 | 4-5 kHz | 256-319 | High wildlife |
| B6 | 5-6 kHz | 320-383 | Very high / bat social (audible) |
| B7 | 6-8 kHz | 384-511 | Upper range / bat social edge |

Use [ArduinoFFT](https://github.com/kosme/arduinoFFT) library.

### 5. Test Sketch (`examples/audio_test/audio_test.ino`)

```cpp
#include "audio_capture.h"
#include "feature_extraction.h"

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  Serial.println("Audio test starting...");
  if (!audio_init()) {
    Serial.println("I2S init failed");
    while (1) delay(1000);
  }
}

void loop() {
  if (!audio_ready()) return;
  int16_t* frame = audio_get_frame();
  if (!frame) { audio_clear_ready(); return; }

  // Compute peak
  int32_t peak = 0;
  for (size_t i = 0; i < AUDIO_FRAME_SAMPLES; i++) {
    int32_t s = abs(frame[i]);
    if (s > peak) peak = s;
  }
  Serial.print("peak="); Serial.println(peak);

  // Extract features
  float features[8];
  if (extract_features(frame, AUDIO_FRAME_SAMPLES, features)) {
    print_features(features);
  }

  audio_clear_ready();
}
```

## Expected Outputs

- `src/audio_capture.h` and `src/audio_capture.cpp` — working I2S capture
- `src/feature_extraction.h` and `src/feature_extraction.cpp` — FFT + 8-band features
- `examples/audio_test/` — standalone test sketch

## Verification

- [ ] `audio_init()` returns true (no I2S init failure)
- [ ] Peak amplitude changes when clapping/tapping near mic
- [ ] Feature vector prints 8 meaningful float values per frame
- [ ] Frame rate: ~15.6 fps (1000 ms / 64 ms per frame)
- [ ] No buffer overruns after 60 seconds of continuous capture
