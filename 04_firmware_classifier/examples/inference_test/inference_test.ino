// CavitySense — Classifier Test Sketch
// Tests Edge Impulse inference: captures audio, runs classifier, prints results
// Flash: make flash-ml-test

#include "audio_capture.h"
#include "cavitysense_classifier.h"
#include "event_protocol.h"
#include "cavitysense_display.h"

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  Serial.println("CavitySense Classifier Test");

  display_init();

  if (!audio_init()) {
    Serial.println("audio_init FAILED");
    while (1) delay(1000);
  }

  classifier_init();
  Serial.println("Ready — making sounds to test classification");
}

void loop() {
  display_startup_graph(millis());

  if (!audio_ready()) return;

  int16_t* frame = audio_get_frame();
  if (!frame) { audio_clear_ready(); return; }

  ClassifierResult result = classifier_run(frame);
  audio_clear_ready();

  Serial.print("Result: ");
  Serial.print(result.label);
  Serial.print(" (");
  Serial.print(result.confidence * 100, 1);
  Serial.println("%)");

  // Drive LED matrix based on classification
  if (strcmp(result.label, "wildlife_activity") == 0 && result.confidence >= CONFIDENCE_THRESHOLD) {
    display_bat_animate(millis());
    emit_event(label_to_event(result.label), result.confidence, millis());
  } else if (strcmp(result.label, "wind_noise") == 0 && result.confidence >= CONFIDENCE_THRESHOLD) {
    display_wind();
  } else {
    display_idle();
  }

  // Periodic heartbeat
  static unsigned long last_hb = 0;
  if (millis() - last_hb > 10000) {
    emit_heartbeat(millis());
    last_hb = millis();
  }
}
