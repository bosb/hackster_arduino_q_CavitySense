# 04 — Firmware: Edge Impulse Classifier Integration

## Purpose

Glue layer that connects audio features from `02_firmware_audio` to the Edge Impulse TinyML model, then drives LED matrix output from `03_firmware_led_matrix` based on classification results.

## Dependencies

- **Input A**: `02_firmware_audio/src/audio_capture.h` and `02_firmware_audio/src/feature_extraction.h`
- **Input B**: `01_ml_pipeline/cavitysense-wildlife_inferencing.zip` (Edge Impulse library, must be installed)
- **Input C**: `03_firmware_led_matrix/src/cavitysense_display.h` and `03_firmware_led_matrix/src/icon_data.h`
- **Output**: combined library consumed by `05_firmware_integration`

## Files to Create

| File | Role |
|------|------|
| `src/cavitysense_classifier.h` | Classifier API: `classifier_init()`, `classifier_run()`, `get_label()`, `get_confidence()` |
| `src/cavitysense_classifier.cpp` | Edge Impulse inference loop, signal callback, label mapping |
| `src/event_protocol.h` | Event types and serialization for IPC |
| `src/event_protocol.cpp` | JSON serialization for Serial1 output |
| `examples/inference_test/inference_test.ino` | Standalone test: run classifier + print results |
| `examples/inference_test/inference_test.yaml` | Sketch metadata |
| `EI_LIBRARY_INSTALL.md` | Instructions for installing the Edge Impulse ZIP library |

## Step-by-Step Implementation

### 1. Install Edge Impulse Library

The exported ZIP from `01_ml_pipeline/` must be installed:

```bash
arduino-cli lib install --zip-path 01_ml_pipeline/cavitysense-wildlife_inferencing.zip
```

This creates a library like `CavitySense-Wildlife_inferencing` in your Arduino libraries folder.

### 2. Edge Impulse Signal Callback

The model expects a `signal_t` with a `get_data` function that provides samples:

```cpp
static int16_t* g_audio_frame = nullptr;

static int get_signal_data(size_t offset, size_t length, float* out_ptr) {
  if (g_audio_frame == nullptr || out_ptr == nullptr) return -1;
  if ((offset + length) > AUDIO_FRAME_SAMPLES) return -1;
  for (size_t ix = 0; ix < length; ++ix) {
    out_ptr[ix] = static_cast<float>(g_audio_frame[offset + ix]) / 32768.0f;
  }
  return 0;
}
```

### 3. Inference Loop

```cpp
#include <cavitysense-wildlife_inferencing.h>  // actual name depends on Edge Impulse export

constexpr float CONFIDENCE_THRESHOLD = 0.75f;

ClassifierResult classifier_run(int16_t* frame) {
  g_audio_frame = frame;

  signal_t signal;
  signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
  signal.get_data = get_signal_data;

  ei_impulse_result_t result = {0};
  EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
  if (err != EI_IMPULSE_OK) {
    return {"error", 0.0f};
  }

  // Find best label
  const char* best_label = "silence";
  float best_value = -1.0f;
  for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; ++i) {
    if (result.classification[i].value > best_value) {
      best_value = result.classification[i].value;
      best_label = result.classification[i].label;
    }
  }

  if (best_value < CONFIDENCE_THRESHOLD) {
    best_label = "silence";
  }

  return {best_label, best_value};
}
```

### 4. Label → Icon Mapping

```cpp
const char* label_to_event_type(const char* label) {
  if (strcmp(label, "wildlife_activity") == 0) return "wildlife";
  if (strcmp(label, "wind_noise") == 0) return "wind";
  if (strcmp(label, "silence") == 0) return "silence";
  return "unknown";
}
```

### 5. Event Protocol (`event_protocol.h/.cpp`)

For communication to the Linux side via `Serial1`:

```cpp
enum class AcousticEvent { SILENCE, WILDLIFE, WIND, UNKNOWN };

void emit_acoustic_event(AcousticEvent ev, float confidence, unsigned long ts_ms);
void emit_heartbeat(unsigned long uptime_ms);
```

Serializes as JSON newline-delimited:

```json
{"v":1,"event":"wildlife","confidence":0.82,"ts":12345}
{"v":1,"event":"heartbeat","uptime":30000,"ts":30000}
```

### 6. Test Sketch (`examples/inference_test/inference_test.ino`)

```cpp
#include "audio_capture.h"
#include "cavitysense_classifier.h"
#include "cavitysense_display.h"
#include "event_protocol.h"

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  Serial.println("Inference test starting...");

  display_init();
  display_startup_graph(millis());

  if (!audio_init()) {
    Serial.println("I2S init failed");
    while (1) delay(1000);
  }

  Serial.println("Ready");
}

void loop() {
  if (!audio_ready()) return;

  int16_t* frame = audio_get_frame();
  if (!frame) { audio_clear_ready(); return; }

  ClassifierResult result = classifier_run(frame);
  audio_clear_ready();

  Serial.print("Label: ");
  Serial.print(result.label);
  Serial.print(" Confidence: ");
  Serial.println(result.confidence, 4);

  if (strcmp(result.label, "wildlife_activity") == 0 && result.confidence >= 0.75) {
    display_bat();
    emit_acoustic_event(AcousticEvent::WILDLIFE, result.confidence, millis());
  } else if (strcmp(result.label, "wind_noise") == 0 && result.confidence >= 0.75) {
    display_wind();
  } else {
    display_idle();
  }
}
```

## Edge Impulse Library Compatibility Notes for UNO Q

The UNO Q Zephyr LLEXT runtime may not export some libc symbols. When compiling, you may need these shims:

```cpp
extern "C" int* __errno(void) {
  static int shim_errno = 0;
  return &shim_errno;
}
extern "C" long long strtoll(const char* nptr, char** endptr, int base) {
  return (long long)strtol(nptr, endptr, base);
}
```

Add these to the sketch or integration layer only if the compiler reports missing symbols.

## Expected Outputs

- `src/cavitysense_classifier.h` and `src/cavitysense_classifier.cpp`
- `src/event_protocol.h` and `src/event_protocol.cpp`
- `examples/inference_test/` — standalone test sketch
- `EI_LIBRARY_INSTALL.md` — installation instructions

## Verification

- [ ] Edge Impulse library compiles without errors on `arduino:zephyr:unoq`
- [ ] `run_classifier()` returns clean results
- [ ] Confidence threshold correctly filters low-confidence predictions
- [ ] `wildlife_activity` label triggers bat icon
- [ ] `wind_noise` label triggers wind icon
- [ ] `silence` shows idle icon
- [ ] JSON events emitted on `Serial1` for non-silence classes
- [ ] Heartbeat JSON emitted every 10 seconds
