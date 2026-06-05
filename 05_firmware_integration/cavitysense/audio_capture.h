#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <Arduino.h>

#define AUDIO_SAMPLE_RATE   16000
#define AUDIO_FRAME_SAMPLES 1024   // 64 ms at 16 kHz

// ── I2S Pin Mapping (UNO Q specific) ─────────────────────────────
#define PIN_I2S_WS   10    // Word select / LRCLK
#define PIN_I2S_SCK  9     // Bit clock / BCLK
#define PIN_I2S_SD   8     // Serial data in
#define PIN_I2S_LR   7     // Channel select (LOW = left ch)

// ── Audio Capture API ────────────────────────────────────────────
bool audio_init();                  // Init I2S, start capture
bool audio_ready();                 // True when a full frame is available
int16_t* audio_get_frame();        // Pointer to current 1024-sample frame
void audio_clear_ready();          // Release buffer for reuse
void audio_start_frame_capture();  // Start capturing a frame (step-based mode)
bool audio_capture_frame_step(size_t n_samples);  // Capture N samples per call

#endif
