// CavitySense — AI-Powered Wildlife Monitoring for Tree Conservation
// Arduino UNO Q firmware
// INMP441 → I2S → FFT features → Edge Impulse classifier → LED matrix + Bridge IPC
// Pins: D7=L/R, D8=SD(in), D9=SCK, D10=WS
// FQBN: arduino:zephyr:unoq:wait_linux_boot=no

// Suppress EI logging: on Zephyr, ei_printf() writes to Serial which
// interferes with the Bridge MsgPack transport.
void ei_printf(const char* /*format*/, ...) {}
void ei_printf_float(float /*f*/) {}

#include <Arduino_RouterBridge.h>
#include "audio_capture.h"
#include "cavitysense_display.h"
#include "cavitysense_classifier.h"
#include "event_protocol.h"
#include <cstring>

// LLEXT libc shims (required on some UNO Q Zephyr builds)
extern "C" int* __errno(void) {
  static int shim_errno = 0;
  return &shim_errno;
}

// ── Constants ────────────────────────────────────────────────────
#define BRIDGE_INTERVAL_MS     2000u
#define BRIDGE_WARMUP_MS       1500UL
#define WAIT_READY_MS         15000UL

// ── State ────────────────────────────────────────────────────────
static const char*    _label             = "silence";
static float          _confidence        = 0.0f;
static unsigned long  _last_heartbeat    = 0;
static unsigned long  _bridge_warmup_until = 0;
static bool           _ready             = false;
static bool           _bridge_flow_ack   = false;

static void on_bridge_flow_ack(int enabled = 1, int /*ts*/ = 0) {
  _bridge_flow_ack = (enabled != 0);
  if (_bridge_flow_ack) {
    Bridge.notify("bridge_flow_ack_seen", 1, (int)(millis() / 1000));
  }
}

static void emit_acoustic_event(const char* label, float confidence, unsigned long now) {
  if (now < _bridge_warmup_until) return;
  int pct = (int)(confidence * 100.0f + 0.5f);
  Bridge.notify("acoustic_event", label, pct, (int)(now / 1000));
  AcousticEvent ev = label_to_event(label);
  emit_event(ev, confidence, now);
}

static void emit_heartbeat(unsigned long now) {
  if (now < _bridge_warmup_until) return;
  if (now - _last_heartbeat < BRIDGE_INTERVAL_MS) return;
  _last_heartbeat = now;
  if (_bridge_flow_ack) {
    display_with_status(display_get_current_icon());
  } else {
    display_startup_graph(now);
  }
  int pct = (int)(_confidence * 100.0f + 0.5f);
  Bridge.notify("acoustic_event", _label, pct, (int)(now / 1000));
}

// ── Main Audio + Classification Loop ─────────────────────────────
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
  const char* new_label = (result.confidence >= CONFIDENCE_THRESHOLD)
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

  // Emit event if label changed and non-silence
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
    while (1) {
      display_with_status(ICON_ERROR);
      delay(500);
      display_clear();
      delay(500);
    }
  }

  classifier_init();
  audio_start_frame_capture();

  unsigned long now = millis();
  _last_heartbeat = now - BRIDGE_INTERVAL_MS;
  _bridge_warmup_until = now + BRIDGE_WARMUP_MS;
}

void loop() {
  unsigned long now = millis();
  emit_heartbeat(now);
  process_audio_loop();
}
