#include "feature_extraction.h"
#include "audio_capture.h"
#include <arduinoFFT.h>

// FFT size must be >= frame_size; we use 1024
// (matching AUDIO_FRAME_SAMPLES)
#define FFT_SIZE 1024

// Frequency bands for 16 kHz sampling, FFT size 1024:
// Bin i corresponds to frequency = i * sample_rate / FFT_SIZE = i * 15.625 Hz
// Band   Freq range    Bins        Ecological meaning
// B0      0-500 Hz      0-31       Ambient rumble
// B1    500-1000 Hz    32-63       Low noise
// B2     1-2 kHz       64-127      Bird calls / low wildlife
// B3     2-3 kHz      128-191      Mid wildlife
// B4     3-4 kHz      192-255      Mid-high wildlife
// B5     4-5 kHz      256-319      High wildlife
// B5     5-7 kHz      320-447      Very high / bat social audible range
// B7    7-8 kHz       448-511      Upper range

static const uint16_t BAND_START[NUM_FEATURE_BANDS] = {0, 32, 64, 128, 192, 256, 320, 448};
static const uint16_t BAND_END[NUM_FEATURE_BANDS]   = {31, 63, 127, 191, 255, 319, 447, 511};

bool extract_features(const int16_t* frame, size_t frame_size,
                      float out_features[NUM_FEATURE_BANDS]) {
  if (!frame || !out_features || frame_size == 0) return false;

  double vReal[FFT_SIZE];
  double vImag[FFT_SIZE];

  // Copy frame into real array, zero imaginary
  size_t copy_len = (frame_size < FFT_SIZE) ? frame_size : FFT_SIZE;
  for (size_t i = 0; i < copy_len; i++) {
    vReal[i] = (double)frame[i];
    vImag[i] = 0.0;
  }
  for (size_t i = copy_len; i < FFT_SIZE; i++) {
    vReal[i] = 0.0;
    vImag[i] = 0.0;
  }

  // Compute FFT
  ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, FFT_SIZE, AUDIO_SAMPLE_RATE);
  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

  // Extract band energies
  for (int b = 0; b < NUM_FEATURE_BANDS; b++) {
    double energy = 0.0;
    for (int bin = BAND_START[b]; bin <= BAND_END[b] && bin < FFT_SIZE / 2; bin++) {
      energy += vReal[bin] * vReal[bin];
    }
    out_features[b] = (float)energy;
  }

  // Normalize: divide by max band energy
  float max_val = 0.0;
  for (int b = 0; b < NUM_FEATURE_BANDS; b++) {
    if (out_features[b] > max_val) max_val = out_features[b];
  }
  if (max_val > 0.0001f) {
    for (int b = 0; b < NUM_FEATURE_BANDS; b++) {
      out_features[b] /= max_val;
    }
  }

  return true;
}

void print_features(const float features[NUM_FEATURE_BANDS]) {
  Serial.print("features:");
  for (int b = 0; b < NUM_FEATURE_BANDS; b++) {
    Serial.print(' ');
    Serial.print(features[b], 4);
  }
  Serial.println();
}
