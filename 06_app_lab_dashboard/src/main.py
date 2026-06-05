import os
import json
import time
import queue
import threading
from flask import Flask, render_template, Response, jsonify

app = Flask(__name__)


# ── Bridge Handler (simulated for development) ───────────────────
class BridgeHandler:
    def __init__(self):
        self.event_queue = queue.Queue()
        self.current_state = {
            "label": "silence",
            "confidence": 0.0,
            "last_event_ts": 0,
            "events_today": 0,
            "bridge_last_event_ms": 0,
            "uptime": 0,
        }
        self.running = False

    def on_acoustic_event(self, label, confidence_pct, timestamp):
        event = {
            "label": label,
            "confidence": confidence_pct / 100.0,
            "ts": timestamp,
            "type": "acoustic",
        }
        self.event_queue.put(event)
        self.current_state["label"] = label
        self.current_state["confidence"] = confidence_pct / 100.0
        self.current_state["last_event_ts"] = timestamp
        self.current_state["events_today"] += 1
        self.current_state["bridge_last_event_ms"] = timestamp * 1000

    def start(self):
        if self.running:
            return
        self.running = True
        # In production, Bridge.provide("acoustic_event", self.on_acoustic_event)
        # Bridge.begin()
        # For dev, simulate events
        thread = threading.Thread(target=self._simulate, daemon=True)
        thread.start()

    def _simulate(self):
        """Simulate bridge events for development without hardware."""
        labels = [
            ("silence", 0.2),
            ("silence", 0.1),
            ("wildlife_activity", 0.85),
            ("silence", 0.3),
            ("wind_noise", 0.78),
            ("silence", 0.15),
        ]
        idx = 0
        ts = 1000
        while self.running:
            time.sleep(8)
            label, conf = labels[idx % len(labels)]
            ts += 8000
            self.on_acoustic_event(label, int(conf * 100), ts // 1000)
            idx += 1
            self.current_state["uptime"] = ts

    def get_event(self):
        return self.event_queue.get(block=True)

    def get_current_state(self):
        return self.current_state


bridge = BridgeHandler()


# ── Routes ───────────────────────────────────────────────────────
@app.route("/")
def index():
    return render_template("index.html")


@app.route("/events")
def stream_events():
    def event_stream():
        while True:
            event = bridge.get_event()
            yield f"data: {json.dumps(event)}\n\n"

    return Response(event_stream(), mimetype="text/event-stream")


@app.route("/state")
def get_state():
    return jsonify(bridge.get_current_state())


@app.route("/health")
def health():
    return jsonify({"status": "ok"})


# ── Entry Point ──────────────────────────────────────────────────
if __name__ == "__main__":
    bridge.start()
    app.run(host="0.0.0.0", port=7000, debug=False)
