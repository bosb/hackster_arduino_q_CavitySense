#include "cavitysense_classifier.h"
#include "audio_capture.h"

// The Edge Impulse library name depends on the project name used during export.
// If exported as "cavitysense-wildlife", the include is:
//   #include <cavitysense-wildlife_inferencing.h>
// Adjust the include below to match your Edge Impulse export.
// If the model is not available, the code compiles in bypass mode.

#if __has_include(<cavitysense-wildlife_inferencing.h>)
  #include <cavitysense-wildlife_inferencing.h>
  #define EI_AVAILABLE 1
#else
  #define EI_AVAILABLE 0
  #pragma message "cavitysense-wildlife_inferencing.h not found — compiling classifier in bypass mode"
#endif

static int16_t* g_frame = nullptr;

static int get_signal_data(size_t offset, size_t length, float* out_ptr) {
  if (g_frame == nullptr || out_ptr == nullptr) return -1;
  if ((offset + length) > AUDIO_FRAME_SAMPLES) return -1;
  for (size_t ix = 0; ix < length; ++ix) {
    out_ptr[ix] = static_cast<float>(g_frame[offset + ix]) / 32768.0f;
  }
  return 0;
}

void classifier_init() {
  // No special initialization needed
}

ClassifierResult classifier_run(int16_t* frame) {
  if (!frame) {
    return {"error", 0.0f};
  }

  g_frame = frame;

#if EI_AVAILABLE
  signal_t signal;
  signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
  signal.get_data = get_signal_data;

  ei_impulse_result_t result = {0};
  EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
  if (err != EI_IMPULSE_OK) {
    return {"error", 0.0f};
  }

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
#else
  // Bypass: always return silence
  (void)get_signal_data;
  return {"silence", 0.0f};
#endif
}
