# 05 — Firmware: Integration Sketch (Combined Firmware)

## Purpose

Final merged Arduino sketch that combines audio capture (02), LED matrix (03), classifier (04), Bridge IPC, and startup logic into one deployable `.ino` file.

## Dependencies

- `01_ml_pipeline/` → Edge Impulse library ZIP (must be installed in Arduino libraries)
- `02_firmware_audio/` → `audio_capture.h`, `feature_extraction.h`
- `03_firmware_led_matrix/` → `cavitysense_display.h`, `icon_data.h`
- `04_firmware_classifier/` → `cavitysense_classifier.h`, `event_protocol.h`
- **Output**: the flashable firmware blob for the UNO Q

## Files to Create

| File | Role |
|------|------|
| `cavitysense.ino` | Main Arduino sketch (entry point) |
| `cavitysense.yaml` | App Lab sketch metadata |
| `calibration.md` | Guide for adjusting confidence thresholds and feature scaling |
| `test_results.md` | Template for recording end-to-end test results |

## Step-by-Step Implementation

### 1. Main Sketch Structure (`cavitysense.ino`)

```cpp
// CavitySense — AI-Powered Wildlife Monitoring for Tree Conservation
// Arduino UNO Q firmware
// INMP441 → I2S → FFT features → Edge Impulse classifier → LED matrix + Bridge IPC
// Pins: D7=L/R, D8=SD(in), D9=SCK, D10=WS

// Suppress EI logging on Serial (interferes with Bridge router)
void ei_printf(const char* /*format*/, ...) {}
void ei_printf_float(float /*f*/) {}

#include <Arduino_RouterBridge.h>
#include <Arduino_LED_Matrix.h>
#include "audio_capture.h"
#include "cavitysense_display.h"
#include "cavitysense_classifier.h"
#include "event_protocol.h"

// LLEXT libc shims (if needed — add only if compilation fails)
extern "C" int* __errno(void) {
  static int shim_errno = 0;
  return &shim_errno;
}

// ── Constants ────────────────────────────────────────────────────
#define CONFIDENCE_THRESHOLD  0.75f
#define BRIDGE_INTERVAL_MS     2000u
#define BRIDGE_WARMUP_MS       1500UL
#define WAIT_READY_MS         15000UL   // allow app/router to bind before inference

// ── State ────────────────────────────────────────────────────────
static const char*  _label        = "silence";
static float        _confidence   = 0.0f;
static unsigned long _last_heartbeat = 0;
static unsigned long _bridge_warmup_until = 0;
static bool         _ready        = false;
static bool         _bridge_flow_ack = false;

static void on_bridge_flow_ack(int enabled = 1, int /*ts*/ = 0) {
  _bridge_flow_ack = (enabled != 0);
  if (_bridge_flow_ack) {
    Bridge.notify("bridge_flow_ack_seen", 1, (int)(millis() / 1000));
  }
}

static void emit_acoustic_event(const char* label, float confidence, unsigned long now) {
  if (now < _bridge_warmup_until) return;  // skip until warmup done
  // Send via Bridge (for App Lab) + Serial1 (for debug fallback)
  Bridge.notify("acoustic_event", label, (int)(confidence * 100 + 0.5f), (int)(now / 1000));
  AcousticEvent ev = AcousticEvent::SILENCE;
  if (strcmp(label, "wildlife_activity") == 0) ev = AcousticEvent::WILDLIFE;
  else if (strcmp(label, "wind_noise") == 0) ev = AcousticEvent::WIND;
  emit_event(ev, confidence, now);
}

static void emit_heartbeat(unsigned long now) {
  if (now < _bridge_warmup_until) return;
  if (now - _last_heartbeat < BRIDGE_INTERVAL_MS) return;
  _last_heartbeat = now;
  Bridge.notify("acoustic_event", _label, (int)(_confidence * 100 + 0.5f), (int)(now / 1000));
  emit_event(AcousticEvent::SILENCE, 0.0f, now);  // heartbeat on Serial1
}

// ── Main Logic ───────────────────────────────────────────────────
static void process_audio_loop() {
  unsigned long now = millis();

  if (!_ready) {
    if (now < WAIT_READY_MS) {
      display_startup_graph(now);
      return;
    }
    _ready = true;
    _bridge_warmup_until = now + BRIDGE_WARMUP_MS;
  }

  if (!audio_ready()) return;

  int16_t* frame = audio_get_frame();
  if (!frame) { audio_clear_ready(); return; }

  ClassifierResult result = classifier_run(frame);
  audio_clear_ready();

  _confidence = result.confidence;
  const char* new_label = result.confidence >= CONFIDENCE_THRESHOLD
    ? result.label : "silence";

  bool changed = (strcmp(new_label, _label) != 0);
  _label = new_label;

  // Update LED matrix
  if (strcmp(_label, "wildlife_activity") == 0) {
    display_bat_animate(now);
  } else if (strcmp(_label, "wind_noise") == 0) {
    display_wind();
  } else {
    display_idle();
  }

  // Emit event if changed and non-silence
  if (changed && strcmp(_label, "silence") != 0) {
    emit_acoustic_event(_label, _confidence, now);
  }
}

// ── Arduino Entry Points ─────────────────────────────────────────
void setup() {
  display_init();
  display_startup_graph(millis());

  Bridge.begin();
  Bridge.provide("bridge_flow_ack", on_bridge_flow_ack);

  if (!audio_init()) {
    // Show error on LED matrix: flash all LEDs
    while (1) {
      matrix.draw(ICON_ERROR);  // defined in icon_data.h
      delay(500);
      matrix.clear();
      delay(500);
    }
  }

  unsigned long now = millis();
  _last_heartbeat = now - BRIDGE_INTERVAL_MS;
  _bridge_warmup_until = now + BRIDGE_WARMUP_MS;
}

void loop() {
  unsigned long now = millis();
  emit_heartbeat(now);
  process_audio_loop();
}
```

### 2. `cavitysense.yaml`

```yaml
name: cavitysense
icon: 🦇
description: >
  AI-powered wildlife activity monitoring for tree conservation.
  Detects acoustic events via INMP441 microphone and Edge Impulse TinyML.
bricks:
  - arduino:web_ui
```

### 3. `calibration.md`

Document the calibration procedure:

1. Place device in quiet environment → observe `silence` classification
2. Gently blow across mic → observe `wind_noise` activation
3. Snap fingers / jingle keys near mic → observe `wildlife_activity` activation
4. Adjust `CONFIDENCE_THRESHOLD` (default 0.75) if needed:
   - Lower (0.6) → more sensitive, more false positives
   - Higher (0.9) → less sensitive, fewer detections
5. Record feature values per class to set minimum energy threshold

### 4. `test_results.md`

Template:

```markdown
# Integration Test Results

Date: [date]
Firmware version: [git hash]

## Test Conditions
- Environment: [quiet room / outdoor / near tree]
- Distance from mic to sound source: [cm]
- Background noise level: [low / medium / high]

## Classification Results

| Test Sound | Expected | Actual | Confidence | Pass/Fail |
|------------|----------|--------|------------|-----------|
| Silence    | silence  |        |            |           |
| Wind       | wind     |        |            |           |
| Finger snap| wildlife |        |            |           |
| Keys       | wildlife |        |            |           |
| Speech     | silence  |        |            |           |

## LED Matrix Verification

- [ ] Idle icon shows when silent
- [ ] Bat icon animates on wildlife detection
- [ ] Wind icon shows on wind noise
- [ ] Status bar heartbeat pulses

## Bridge IPC Verification

- [ ] Events appear on Serial1 as JSON
- [ ] Heartbeat every 2 seconds
- [ ] Bridge.notify sends acoustic_event to App Lab
```

## Expected Outputs

- `cavitysense.ino` — the deployable firmware
- `cavitysense.yaml` — App Lab metadata
- `calibration.md` — threshold tuning guide
- `test_results.md` — results template

## Verification

- [ ] `./flash.sh` compiles without errors
- [ ] Firmware uploads via remoteocd successfully
- [ ] Device boots → startup graph animation → idle mode
- [ ] Clap/tap → classifier triggers → bat icon + Bridge event
- [ ] LED matrix shows correct icons per classification
- [ ] Serial1 outputs JSON event lines
- [ ] Bridge.notify sends events to the Linux side
