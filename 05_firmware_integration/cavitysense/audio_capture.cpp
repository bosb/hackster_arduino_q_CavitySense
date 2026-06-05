#include "audio_capture.h"

// ── Buffer management ────────────────────────────────────────────
static int16_t _buffer[AUDIO_FRAME_SAMPLES];
static volatile size_t _buffer_pos = 0;
static volatile bool _ready = false;
static bool _capturing = false;

// ── I2S Bit-bang Helpers ─────────────────────────────────────────
// UNO Q Zephyr core may not expose hardware I2S on D8-D10.
// This bit-bang fallback works on any pins.

static inline void i2s_clock_high() { digitalWrite(PIN_I2S_SCK, HIGH); }
static inline void i2s_clock_low()  { digitalWrite(PIN_I2S_SCK, LOW); }
static inline void i2s_word_select(bool left) {
  digitalWrite(PIN_I2S_WS, left ? LOW : HIGH);
}
static inline bool i2s_read_bit() {
  return digitalRead(PIN_I2S_SD);
}

static int32_t i2s_read_sample() {
  int32_t sample = 0;
  // Read 32 bits (MSB first, data in upper 24 bits)
  for (int i = 31; i >= 0; i--) {
    if (i2s_read_bit()) {
      sample |= (1UL << i);
    }
    i2s_clock_high();
    i2s_clock_low();
  }
  // Shift right by 8 to get 24-bit signed value, then scale to int16
  return sample >> 8;
}

// ── Audio Capture Implementation ─────────────────────────────────

bool audio_init() {
  pinMode(PIN_I2S_WS, OUTPUT);
  pinMode(PIN_I2S_SCK, OUTPUT);
  pinMode(PIN_I2S_SD, INPUT);
  pinMode(PIN_I2S_LR, OUTPUT);

  // D7 LOW = left channel selected
  digitalWrite(PIN_I2S_LR, LOW);

  // Idle clock LOW, word select LOW (left channel)
  i2s_clock_low();
  i2s_word_select(true);

  _buffer_pos = 0;
  _ready = false;
  _capturing = false;

  return true;
}

void audio_start_frame_capture() {
  _buffer_pos = 0;
  _ready = false;
  _capturing = true;
}

bool audio_capture_frame_step(size_t n_samples) {
  if (!_capturing || _ready) return false;

  for (size_t i = 0; i < n_samples && _buffer_pos < AUDIO_FRAME_SAMPLES; i++) {
    int32_t raw = i2s_read_sample();
    // Clamp to int16 range
    if (raw > 32767) raw = 32767;
    if (raw < -32768) raw = -32768;
    _buffer[_buffer_pos++] = (int16_t)raw;
  }

  if (_buffer_pos >= AUDIO_FRAME_SAMPLES) {
    _ready = true;
    _capturing = false;
  }

  return _ready;
}

bool audio_ready() {
  return _ready;
}

int16_t* audio_get_frame() {
  if (!_ready) return nullptr;
  return _buffer;
}

void audio_clear_ready() {
  _ready = false;
  _buffer_pos = 0;
}
