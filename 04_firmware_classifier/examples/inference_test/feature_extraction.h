#ifndef FEATURE_EXTRACTION_H
#define FEATURE_EXTRACTION_H

#include <Arduino.h>

#define NUM_FEATURE_BANDS 8

// Extract 8 normalized frequency band energy features from a 1024-sample frame
// sampled at 16 kHz. Uses ArduinoFFT internally.
// Returns false on error (e.g. null pointer).
bool extract_features(const int16_t* frame, size_t frame_size,
                      float out_features[NUM_FEATURE_BANDS]);

// Print feature vector to Serial for debugging
void print_features(const float features[NUM_FEATURE_BANDS]);

#endif
