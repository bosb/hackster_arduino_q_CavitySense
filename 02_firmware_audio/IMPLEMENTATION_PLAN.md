# 02 — Firmware: ELV MEMS1 Audio Capture + Feature Extraction

## Purpose

Analog audio capture from ELV MEMS1 microphone at 16 kHz, 1024-sample frames, with FFT-based feature extraction. Exposes a clean API (`audio_capture.h`) used by the classifier and integration layers.

## Dependencies

- Hardware: ELV MEMS1 wired per `07_hardware/` (A0 analog in)
- **Output**: library files consumed by `04_firmware_classifier` and `05_firmware_integration`

## Files to Create

| File | Role |
|------|------|
| `src/audio_capture.h` | Audio buffer API: `audio_init()`, `audio_ready()`, `audio_get_frame()`, `audio_clear_ready()` |
| `src/audio_capture.cpp` | Analog ADC backend: GPT timer (TIM6) + continuous ADC capture |
| `src/feature_extraction.h` | FFT + 8-band energy feature extraction |
| `src/feature_extraction.cpp` | ArduinoFFT wrapper, band energy computation, normalization |
| `examples/audio_test/audio_test.ino` | Standalone test sketch: capture + print peak levels |
| `examples/audio_test/audio_test.yaml` | App Lab sketch metadata (for standalone flash) |

## Step-by-Step Implementation

### 1. Analog Pin Configuration

| Function | UNO Q Pin |
|----------|-----------|
| Audio in (ELV MEMS1 OUT) | **A0** |
| Power (ELV MEMS1 VDD) | **3.3V** |
| Ground (ELV MEMS1 GND) | **GND** |

### 2. `audio_capture.h` API

```cpp
#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <Arduino.h>

#define AUDIO_SAMPLE_RATE   16000
#define AUDIO_FRAME_SAMPLES 1024   // 64 ms at 16 kHz

bool audio_init();                  // Init ADC, calibrate bias, start GPT timer capture
bool audio_ready();                 // True when a full frame is available
int16_t* audio_get_frame();        // Pointer to current 1024-sample frame
void audio_clear_ready();          // Release buffer for reuse

#endif
```

### 3. `audio_capture.cpp` — ADC + GPT Timer Backend

Key requirements:
- Sample rate: **16 kHz** mono (62.5 µs period)
- GPT timer: TIM6 (basic timer at 0x40001000, PSC=0, ARR=9999 → 160 MHz / 10000 = 16 kHz)
- ADC: continuous conversion mode via STM32 LL driver, reads data register in ISR
- Bias calibration: 128 synchronous analogRead samples averaged before timer starts
- Buffer: single-buffer 1024 samples, ISR writes until full, then sets `_ready = true`
- Frame trigger: TIM6 update interrupt registered via `irq_connect_dynamic()`

ISR:

```cpp
static void tim6_isr(const void*) {
  if (!(TIM6->SR & TIM_SR_UIF)) return;
  TIM6->SR = ~TIM_SR_UIF;
  int32_t sample = (int32_t)LL_ADC_REG_ReadConversionData12(ADC1) - _bias;
  if (sample < -32768) sample = -32768;
  if (sample > 32767) sample = 32767;
  if (!_ready) {
    _buffer[_buffer_pos++] = (int16_t)sample;
    if (_buffer_pos >= AUDIO_FRAME_SAMPLES) {
      _buffer_pos = 0;
      _ready = true;
    }
  }
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
    Serial.println("ADC init failed");
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

- `src/audio_capture.h` and `src/audio_capture.cpp` — working ADC + GPT capture
- `src/feature_extraction.h` and `src/feature_extraction.cpp` — FFT + 8-band features
- `examples/audio_test/` — standalone test sketch

## Verification

- [ ] `audio_init()` returns true (ADC + timer init succeeds)
- [ ] Bias calibration: idle reading ~0 ±100 counts
- [ ] Peak amplitude changes when clapping/tapping near mic
- [ ] Feature vector prints 8 meaningful float values per frame
- [ ] Frame rate: ~15.6 fps (1000 ms / 64 ms per frame)
- [ ] No buffer overruns after 60 seconds of continuous capture
