#include "cavitysense_display.h"

static ArduinoLEDMatrix _matrix;
static bool _heartbeat = false;
static const uint8_t* _current_icon = ICON_IDLE;

void display_init() {
  _matrix.begin();
  _matrix.clear();
}

void display_clear() {
  _matrix.clear();
}

void display_with_status(const uint8_t icon[104]) {
  uint8_t frame[DISPLAY_PIXELS];
  memcpy(frame, icon, DISPLAY_PIXELS);

  _heartbeat = !_heartbeat;
  uint8_t hb = _heartbeat ? 5 : 2;

  for (int r = 0; r < DISPLAY_ROWS; r++) {
    // Col 10: dim separator
    frame[r * 13 + 10] = 1;
    // Cols 11-12: heartbeat (alternates 2↔5)
    frame[r * 13 + 11] = hb;
    frame[r * 13 + 12] = hb;
  }

  _matrix.draw(frame);
}

const uint8_t* display_get_current_icon() {
  return _current_icon;
}

void display_idle() {
  _current_icon = ICON_IDLE;
  display_with_status(ICON_IDLE);
}

void display_bat() {
  _current_icon = ICON_BAT;
  display_with_status(ICON_BAT);
}

void display_bat_animate(unsigned long now_ms) {
  if ((now_ms / 200) % 2 == 0) {
    _current_icon = ICON_BAT_UP;
    display_with_status(ICON_BAT_UP);
  } else {
    _current_icon = ICON_BAT_DOWN;
    display_with_status(ICON_BAT_DOWN);
  }
}

void display_wind() {
  _current_icon = ICON_WIND;
  display_with_status(ICON_WIND);
}

void display_tree() {
  _current_icon = ICON_TREE;
  display_with_status(ICON_TREE);
}

void display_startup_graph(unsigned long now_ms) {
  uint8_t frame[DISPLAY_PIXELS] = {0};
  int phase = (int)((now_ms / 120) % 10u);

  for (int col = 0; col < 10; ++col) {
    uint8_t h = STARTUP_BARS[(col + phase) % 10];
    for (int row = 7; row >= (8 - (int)h); --row) {
      frame[row * 13 + col] = (row <= 2) ? 4 : 6;
    }
  }

  _heartbeat = !_heartbeat;
  uint8_t hb = _heartbeat ? 5 : 2;
  for (int r = 0; r < DISPLAY_ROWS; ++r) {
    frame[r * 13 + 10] = 1;
    if (r < 4) {
      frame[r * 13 + 11] = 5;
      frame[r * 13 + 12] = 5;
    } else if (r < 6) {
      frame[r * 13 + 11] = 2;
      frame[r * 13 + 12] = 2;
    } else {
      frame[r * 13 + 11] = hb;
      frame[r * 13 + 12] = hb;
    }
  }

  _matrix.draw(frame);
}

void display_scale_brightness(const uint8_t src[104], uint8_t dst[104], uint8_t level) {
  for (int i = 0; i < DISPLAY_PIXELS; i++) {
    dst[i] = (src[i] * level) / 7;
  }
}
