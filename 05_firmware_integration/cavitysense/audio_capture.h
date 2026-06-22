#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <Arduino.h>

#define AUDIO_SAMPLE_RATE   16000
#define AUDIO_FRAME_SAMPLES 1024   // 64 ms at 16 kHz

// ── Analog Microphone Pin (ELV MEMS1 / SPU0410LR5H-QB) ──────────
#define PIN_MIC_ANALOG  A0

// ── Audio Capture API ────────────────────────────────────────────
bool audio_init();                  // Init ADC + GPT timer, start capture
bool audio_ready();                 // True when a full frame is available
int16_t* audio_get_frame();        // Pointer to current 1024-sample frame
void audio_clear_ready();          // Release buffer for reuse

#endif
