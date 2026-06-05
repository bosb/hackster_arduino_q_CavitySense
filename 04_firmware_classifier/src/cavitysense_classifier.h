#ifndef CAVITYSENSE_CLASSIFIER_H
#define CAVITYSENSE_CLASSIFIER_H

#include <Arduino.h>

#define CONFIDENCE_THRESHOLD 0.75f

struct ClassifierResult {
  const char* label;      // "silence", "wind_noise", "wildlife_activity", or "error"
  float       confidence; // 0.0 - 1.0
};

// ── Lifecycle ───────────────────────────────────────────────────
void classifier_init();

// ── Inference ───────────────────────────────────────────────────
// Takes a 1024-sample int16 audio frame, runs Edge Impulse classifier,
// returns the best label and confidence.
ClassifierResult classifier_run(int16_t* frame);

#endif
