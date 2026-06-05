// CavitySense — LED Matrix Test Sketch
// Cycles through all icons and animations
// Flash: make flash-led-test

#include "cavitysense_display.h"

ArduinoLEDMatrix matrix;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  Serial.println("LED Matrix Test");

  display_init();
}

void loop() {
  Serial.println("IDLE");
  display_idle();
  delay(2000);

  Serial.println("BAT (static)");
  display_bat();
  delay(2000);

  Serial.println("BAT (animated)");
  for (int i = 0; i < 10; i++) {
    display_bat_animate(millis());
    delay(200);
  }

  Serial.println("WIND");
  display_wind();
  delay(2000);

  Serial.println("TREE");
  display_tree();
  delay(2000);

  Serial.println("STARTUP GRAPH");
  for (int i = 0; i < 30; i++) {
    display_startup_graph(millis());
    delay(120);
  }

  Serial.println("CLEAR");
  display_clear();
  delay(1000);
}
