#ifndef EVENT_PROTOCOL_H
#define EVENT_PROTOCOL_H

#include <Arduino.h>

// ── Event Types ─────────────────────────────────────────────────
enum class AcousticEvent {
  SILENCE,     // No activity (used for heartbeat only, never emitted)
  WILDLIFE,    // wildlife_activity class detected
  WIND,        // wind_noise class detected
  UNKNOWN      // Fallback
};

// ── Serialization ───────────────────────────────────────────────
// Emits a JSON event on Serial1 (Linux UART):
//   {"v":1,"event":"wildlife","confidence":0.82,"ts":12345}
void emit_event(AcousticEvent ev, float confidence, unsigned long ts_ms);

// Emits a heartbeat JSON:
//   {"v":1,"event":"heartbeat","uptime":30000,"ts":30000}
void emit_heartbeat(unsigned long uptime_ms);

// Convert string label to AcousticEvent
AcousticEvent label_to_event(const char* label);
const char* event_to_string(AcousticEvent ev);

#endif
