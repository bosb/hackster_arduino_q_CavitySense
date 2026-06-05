#ifndef CAVITYSENSE_DISPLAY_H
#define CAVITYSENSE_DISPLAY_H

#include <Arduino.h>
#include <Arduino_LED_Matrix.h>
#include "icon_data.h"

#define DISPLAY_COLS 13
#define DISPLAY_ROWS 8
#define DISPLAY_PIXELS 104

// ── Initialization ───────────────────────────────────────────────
void display_init();

// ── Icon displays ────────────────────────────────────────────────
void display_idle();
void display_bat();
void display_bat_animate(unsigned long now_ms);
void display_wind();
void display_tree();
void display_clear();

// ── Startup animation ────────────────────────────────────────────
void display_startup_graph(unsigned long now_ms);

// ── Utility ──────────────────────────────────────────────────────
void display_with_status(const uint8_t icon[104]);
void display_scale_brightness(const uint8_t src[104], uint8_t dst[104], uint8_t level);
const uint8_t* display_get_current_icon();

// ── Brightness mapping (0-7) ─────────────────────────────────────
#define BRIGHTNESS_FULL   7
#define BRIGHTNESS_HIGH   5
#define BRIGHTNESS_MED    3
#define BRIGHTNESS_DIM    1
#define BRIGHTNESS_OFF    0

#endif
