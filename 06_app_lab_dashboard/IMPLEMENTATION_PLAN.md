# 06 — App Lab Dashboard: Python Flask Wildlife Monitor

## Purpose

Web-based dashboard deployed on the Arduino UNO Q Linux side via App Lab. Receives acoustic events from the MCU via Bridge, displays current status, activity history, and conservation-relevant visualizations.

## Dependencies

- Hardware: Arduino UNO Q (Linux co-processor, WiFi, App Lab)
- MCU firmware from `05_firmware_integration/` (Bridge IPC)
- **Independent** of other modules — can be developed and tested on any machine with Python

## Files to Create

| File | Role |
|------|------|
| `app.yaml` | App Lab app metadata |
| `src/main.py` | Flask web server entry point |
| `src/bridge_handler.py` | Bridge event listener (receives MCU classification events) |
| `src/static/index.html` | Dashboard UI (HTML + CSS + JS) |
| `src/static/style.css` | Dashboard styling |
| `src/static/app.js` | Dashboard logic (SSE updates, charts) |
| `src/templates/` | Jinja2 templates for dashboard pages |
| `requirements.txt` | Python dependencies |
| `IMPLEMENTATION.md` | Setup and deployment guide |

## Step-by-Step Implementation

### 1. App Lab Metadata (`app.yaml`)

```yaml
name: cavitysense-dashboard
icon: 🦇
description: >
  CavitySense wildlife activity monitoring dashboard.
  Real-time acoustic event visualization for tree conservation.
ports:
  - 7000
bricks:
  - arduino:web_ui
```

### 2. Flask Server (`src/main.py`)

```python
import os
import json
from flask import Flask, render_template, Response, jsonify
from bridge_handler import BridgeHandler

app = Flask(__name__)
bridge = BridgeHandler()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/events')
def stream_events():
    def event_stream():
        while True:
            event = bridge.get_event()  # blocking, yields JSON lines
            yield f"data: {json.dumps(event)}\n\n"
    return Response(event_stream(), mimetype='text/event-stream')

@app.route('/state')
def get_state():
    return jsonify(bridge.get_current_state())

@app.route('/health')
def health():
    return jsonify({"status": "ok"})

if __name__ == '__main__':
    bridge.start()
    app.run(host='0.0.0.0', port=7000, debug=False)
```

### 3. Bridge Handler (`src/bridge_handler.py`)

```python
import queue
import threading
import json
from arduino_router_bridge import Bridge  # App Lab Bridge SDK

class BridgeHandler:
    def __init__(self):
        self.event_queue = queue.Queue()
        self.current_state = {
            "label": "silence",
            "confidence": 0.0,
            "last_event_ts": 0,
            "events_today": 0,
            "bridge_last_event_ms": 0
        }
        self.running = False

    def on_acoustic_event(self, label, confidence_pct, timestamp):
        event = {
            "label": label,
            "confidence": confidence_pct / 100.0,
            "ts": timestamp,
            "type": "acoustic"
        }
        self.event_queue.put(event)
        self.current_state["label"] = label
        self.current_state["confidence"] = confidence_pct / 100.0
        self.current_state["last_event_ts"] = timestamp
        self.current_state["events_today"] += 1
        self.current_state["bridge_last_event_ms"] = timestamp * 1000

    def start(self):
        if not self.running:
            # Register Bridge callback
            Bridge.provide("acoustic_event", self.on_acoustic_event)
            Bridge.begin()
            self.running = True
            # Start background listener
            thread = threading.Thread(target=self._listen, daemon=True)
            thread.start()

    def _listen(self):
        while self.running:
            # Bridge event loop
            Bridge.loop()
            time.sleep(0.01)

    def get_event(self):
        return self.event_queue.get(block=True)

    def get_current_state(self):
        return self.current_state
```

### 4. Dashboard UI (`src/templates/index.html`)

HTML page with:

- **Header**: CavitySense title + bat icon (CSS animated)
- **Status Card**: Current classification label + confidence gauge
- **Activity Score**: Large circular gauge (0-100%)
- **Event Log**: Scrollable list of recent detections with timestamps
- **Tree Status**: Tree icon showing monitoring state
- **Daily Summary**: Event count for today

### 5. Frontend Logic (`src/static/app.js`)

```javascript
const eventSource = new EventSource('/events');

eventSource.onmessage = function(event) {
    const data = JSON.parse(event.data);
    updateDisplay(data);
};

function updateDisplay(data) {
    document.getElementById('label').textContent = data.label;
    document.getElementById('confidence').textContent =
        (data.confidence * 100).toFixed(0) + '%';

    if (data.label === 'wildlife_activity') {
        showBatIcon();
    } else if (data.label === 'wind_noise') {
        showWindIcon();
    } else {
        showIdleIcon();
    }

    addLogEntry(data);
}
```

### 6. Dashboard Styling (`src/static/style.css`)

- Dark theme (forest/nature tone — green/dark gray)
- Bat icon animation: SVG or CSS-based flapping
- Responsive layout for mobile/desktop
- Smooth transitions on state changes

### 7. `requirements.txt`

```
flask>=2.0
arduino-router-bridge  # provided by App Lab runtime
gunicorn>=20.0
```

## Deployment via App Lab

The dashboard is deployed as an App Lab app:

```bash
# Package and upload
cd 06_app_lab_dashboard
zip -r cavitysense-dashboard.zip src/ app.yaml requirements.txt

# Upload via app-cli (after SSH to UNO Q)
curl -X POST http://localhost:8800/v1/apps/import -F "file=@cavitysense-dashboard.zip"
curl -X POST http://localhost:8800/v1/apps/<id>/start
```

Or use the automated deploy flow from `09_scripts/deploy.sh`.

## Expected Outputs

- `app.yaml` — App Lab metadata
- `src/main.py` — Flask server
- `src/bridge_handler.py` — Bridge event integration
- `src/templates/index.html` — Dashboard HTML
- `src/static/app.js` — Frontend logic
- `src/static/style.css` — Dashboard styling
- `requirements.txt` — Python dependencies

## Verification

- [ ] Flask starts and serves on port 7000
- [ ] SSE stream delivers events when MCU sends via Bridge
- [ ] Dashboard shows correct label and confidence
- [ ] UI updates in real-time without page refresh
- [ ] Event log grows with each detection
- [ ] Daily event counter increments
- [ ] Responsive layout works on mobile
- [ ] Health endpoint returns 200 OK
