#include "event_protocol.h"
#include <cstring>
#include <cstdio>

AcousticEvent label_to_event(const char* label) {
  if (label == nullptr) return AcousticEvent::UNKNOWN;
  if (strcmp(label, "wildlife_activity") == 0) return AcousticEvent::WILDLIFE;
  if (strcmp(label, "wind_noise") == 0) return AcousticEvent::WIND;
  if (strcmp(label, "silence") == 0) return AcousticEvent::SILENCE;
  return AcousticEvent::UNKNOWN;
}

const char* event_to_string(AcousticEvent ev) {
  switch (ev) {
    case AcousticEvent::WILDLIFE: return "wildlife";
    case AcousticEvent::WIND:     return "wind";
    case AcousticEvent::SILENCE:  return "silence";
    default:                      return "unknown";
  }
}

void emit_event(AcousticEvent ev, float confidence, unsigned long ts_ms) {
  // Only emit non-silence events on Serial1 (Linux IPC)
  if (ev == AcousticEvent::SILENCE) return;

  char buf[128];
  snprintf(buf, sizeof(buf),
    "{\"v\":1,\"event\":\"%s\",\"confidence\":%.2f,\"ts\":%lu}\n",
    event_to_string(ev), confidence, ts_ms);

  Serial1.print(buf);
}

void emit_heartbeat(unsigned long uptime_ms) {
  char buf[128];
  snprintf(buf, sizeof(buf),
    "{\"v\":1,\"event\":\"heartbeat\",\"uptime\":%lu,\"ts\":%lu}\n",
    uptime_ms, uptime_ms);

  Serial1.print(buf);
}
