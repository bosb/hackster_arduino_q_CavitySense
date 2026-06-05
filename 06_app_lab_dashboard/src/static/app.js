// CavitySense Dashboard — Frontend Logic
const eventSource = new EventSource('/events');
const labelDisplay = document.getElementById('labelDisplay');
const confidenceDisplay = document.getElementById('confidenceDisplay');
const gaugeFill = document.getElementById('gaugeFill');
const eventLog = document.getElementById('eventLog');
const statusDot = document.getElementById('statusDot');
const eventsToday = document.getElementById('eventsToday');
const uptimeDisplay = document.getElementById('uptime');

let eventCount = 0;

eventSource.onmessage = function(event) {
  const data = JSON.parse(event.data);
  updateDisplay(data);
};

eventSource.onerror = function() {
  console.log('SSE connection lost, retrying...');
};

function updateDisplay(data) {
  // Label and confidence
  labelDisplay.textContent = data.label;
  const pct = Math.round(data.confidence * 100);
  confidenceDisplay.textContent = pct + '%';

  // Status dot
  statusDot.className = 'status-indicator';
  if (data.label === 'wildlife_activity') {
    statusDot.classList.add('wildlife');
  } else if (data.label === 'wind_noise') {
    statusDot.classList.add('wind');
  } else {
    statusDot.classList.add('idle');
  }

  // Gauge
  gaugeFill.style.width = pct + '%';

  // Event count
  eventCount++;
  eventsToday.textContent = eventCount;

  // Log
  addLogEntry(data);

  // Uptime
  if (data.ts) {
    uptimeDisplay.textContent = formatDuration(data.ts);
  }
}

function addLogEntry(data) {
  const entry = document.createElement('div');
  entry.className = 'log-entry';

  const labelSpan = document.createElement('span');
  labelSpan.className = 'ev-label ' + data.label;
  labelSpan.textContent = data.label;

  const timeSpan = document.createElement('span');
  timeSpan.className = 'ev-time';
  timeSpan.textContent = data.ts ? formatTimestamp(data.ts) : '--';

  entry.appendChild(labelSpan);
  entry.appendChild(timeSpan);

  // Remove placeholder
  const placeholder = eventLog.querySelector('.log-placeholder');
  if (placeholder) placeholder.remove();

  eventLog.appendChild(entry);

  // Keep only last 50 entries
  while (eventLog.children.length > 50) {
    eventLog.removeChild(eventLog.firstChild);
  }
}

function formatDuration(seconds) {
  const mins = Math.floor(seconds / 60);
  const secs = seconds % 60;
  if (mins > 0) return `${mins}m ${secs}s`;
  return `${secs}s`;
}

function formatTimestamp(seconds) {
  const d = new Date(seconds * 1000);
  return d.toLocaleTimeString();
}
