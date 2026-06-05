# System Architecture

## High-Level Overview

```text
[Tree Cavity Sound]
↓
[INMP441 Digital Microphone]
↓  I2S (16 kHz mono, 32-bit frames)
[Arduino UNO Q — MCU Core (STM32)]
├── Audio Capture (double-buffered ping-pong)
├── FFT Feature Extraction (8 frequency bands)
├── Edge Impulse TinyML Classifier (3 classes)
├── LED Matrix Visualization (13×8 grayscale)
└── Bridge IPC (JSON over Serial1)
↓
[Arduino UNO Q — Linux Core]
├── App Lab Dashboard (Flask web server)
├── WiFi (client or AP mode)
└── SSE push to browser
↓
[Browser / Mobile Dashboard]
```

## Data Flow

1. **Acquisition**: INMP441 captures audio → I2S interface → 1024-sample frames at 16 kHz (64ms windows)
2. **Feature Extraction**: FFT → 8 frequency band energy features (0-500Hz, 500-1kHz, 1-2kHz, 2-3kHz, 3-4kHz, 4-5kHz, 5-7kHz, 7-8kHz)
3. **Classification**: Edge Impulse MFCC → TinyML neural network → label + confidence
4. **Visualization**: LED matrix shows icon based on class (bat for wildlife, wavy lines for wind, ring for idle)
5. **Reporting**: Bridge IPC sends JSON events to Linux side → Flask dashboard serves SSE to browser

## Hardware Stack

| Layer | Component |
|-------|-----------|
| Sensor | INMP441 MEMS microphone (I2S digital output) |
| MCU | Arduino UNO Q STM32U585 (Zephyr RTOS) |
| Display | 13×8 grayscale LED matrix (built-in) |
| Linux | Arduino UNO Q Linux co-processor |
| Connectivity | WiFi (Linux side) |

## Software Stack

| Layer | Technology |
|-------|------------|
| Audio Driver | Bit-bang I2S on D7-D10 |
| Signal Processing | ArduinoFFT library |
| ML Framework | Edge Impulse (MFCC + Dense NN, INT8 quantized) |
| Firmware | Arduino sketch (Zephyr LLEXT) |
| Dashboard | Python Flask + SSE |
| Deployment | App Lab (arduino-app-cli) |
