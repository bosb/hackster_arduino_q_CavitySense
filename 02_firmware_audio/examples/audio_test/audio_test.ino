// CavitySense — Audio Capture + FFT Test Sketch
// Captures audio via ELV MEMS1 analog mic, computes FFT features, prints to serial.
// Flash: make flash-audio-test

#include "audio_capture.h"
#include "feature_extraction.h"

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  Serial.println("CavitySense Audio Test");

  if (!audio_init()) {
    Serial.println("audio_init FAILED");
    while (1) delay(1000);
  }

  Serial.println("Audio capture running — waiting for frames...");
}

void loop() {
  if (!audio_ready()) return;

  int16_t* frame = audio_get_frame();
  if (!frame) { audio_clear_ready(); return; }

  // Compute peak for quick visual check
  int32_t peak = 0;
  for (size_t i = 0; i < AUDIO_FRAME_SAMPLES; i++) {
    int32_t s = abs(frame[i]);
    if (s > peak) peak = s;
  }

  // Extract FFT features
  float features[NUM_FEATURE_BANDS];
  if (extract_features(frame, AUDIO_FRAME_SAMPLES, features)) {
    Serial.print("peak=");
    Serial.print(peak);
    Serial.print(' ');
    print_features(features);
  } else {
    Serial.println("feature extraction failed");
  }

  audio_clear_ready();
}
