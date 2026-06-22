# CavitySense: AI-Powered Wildlife Monitoring for Tree Conservation

## The Problem

When trees are removed for railway electrification, housing developments, or road widening, authorities must first check for protected wildlife -- bats, birds, and small mammals living in tree cavities. Professional bat detectors cost hundreds to thousands of euros and require expert training to operate. This means either trees are checked slowly and expensively, or wildlife habitat is lost without ever being surveyed.

This project was inspired by a real case: during railway electrification in Germany, conservation groups fought to save bat habitats along the route. There had to be a cheaper, simpler way to pre-screen trees at scale.

## The Solution: CavitySense

CavitySense is a low-cost acoustic monitoring node that attaches to a tree and autonomously listens for signs of wildlife activity inside cavities. It runs entirely on the edge -- no cloud, no data transmission, no subscription. Just a MEMS microphone, an Arduino UNO Q, and a TinyML model.

Total parts cost: ~10 EUR (excluding the UNO Q).

## What It Does

The device listens through a tiny analog MEMS microphone (ELV MEMS1, ~3 EUR) at the entrance of a tree cavity. Audio is sampled at 16 kHz and processed in 64-millisecond frames. On each frame, an FFT extracts 8 frequency band features, and an Edge Impulse TinyML neural network classifies the result into one of three categories:

- Silence -- no activity
- Wind noise -- environmental sound, not animals
- Wildlife activity -- sounds consistent with bats, birds, or small mammals

When wildlife is detected, an animated bat icon appears on the built-in LED matrix. Wind noise shows wavy lines. An idle pulsing ring means the tree is quiet. The entire classification happens on-device in real time.

## How It Works

The Arduino UNO Q is a unique dual-core board. CavitySense uses both cores:

**MCU Core (STM32U585, Zephyr RTOS):** Captures audio from the MEMS microphone via the ADC at precisely 16 kHz (timed by a hardware GPT timer). Runs FFT feature extraction and Edge Impulse inference. Drives the LED matrix for instant visual feedback. Sends JSON event messages over an internal serial link to the Linux core.

**Linux Core:** Runs a Flask web server that receives events and pushes them to browsers via SSE (Server-Sent Events). Serves a mobile dashboard accessible from any phone on the local network via WiFi (AP or client mode). Handles networking without burdening the real-time audio loop.

This dual-core architecture is a key innovation: deterministic low-latency audio processing on the MCU, with a full web server on Linux -- no compromises needed.

## AI Integration

The AI pipeline is built with Edge Impulse:

1. Training data was collected at 16 kHz mono (WAV files) covering three classes: silence, wind noise, and wildlife-like sounds (finger snaps, key jingles, paper crinkling, tapping)
2. MFCC preprocessing extracts spectral features from 1-second windows
3. A small dense neural network (16 -> 8 -> 3 neurons, INT8 quantized) runs inference in under 64ms
4. The quantized model is ~10-20 KB, well within the UNO Q's 128 KB RAM

No audio data ever leaves the device. All inference is on the edge.

## User Experience

**On-site:** Power on via USB-C. The LED matrix immediately shows a startup animation, then settles into a pulsing ring. No phone, no app, no training needed. Anyone can tell at a glance whether a tree has activity.

**Remote:** Open a browser on any device connected to the UNO Q's WiFi. The dashboard shows the current classification, confidence score as a percentage, and a scrollable history of detections with timestamps. All updates push in real time via SSE.

The confidence threshold is configurable in firmware -- from 0.5 (sensitive, more detections) to 0.9 (only very certain events trigger alerts).

## Sustainability

- **Preventative screening:** Helps identify inhabited trees before removal, directly reducing unnecessary habitat destruction
- **Edge processing:** No cloud computation or data transmission -- just the MCU running continuously at low power
- **Minimal hardware:** A microcontroller, a thumbnail-sized microphone, and a USB power bank
- **Targeted intervention:** Professional ecologists only need to follow up on trees flagged by the AI, saving time and money

## Scalability

The same hardware can be deployed across dozens of trees. Each node is independent. Data can be aggregated into activity heatmaps for forest-scale planning. The model can be retrained for different species or environments. Future enhancements could include LoRa mesh networking, SD card logging, or solar power.

## Built With

- Arduino UNO Q (STM32U585 + Linux co-processor)
- ELV MEMS1 analog MEMS microphone (Knowles SPU0410LR5H-QB)
- Edge Impulse TinyML
- ArduinoFFT
- Python Flask + App Lab
- Zephyr RTOS (Arduino core)

## BOM (Minimal Prototype)

- Arduino UNO Q -- 1 (provided by contest)
- ELV MEMS1 analog MEMS microphone -- 1 (~3-5 EUR)
- Jumper wires (female-female, 15 cm) -- 3 (~0.50 EUR)
- Half-size breadboard -- 1 (~3 EUR)
- USB-C cable -- 1 (~3 EUR)

Total: ~9.50 EUR

## Repository

The full source code, wiring diagrams, test results, and documentation are available on GitHub.

## Next Steps

- Longer field testing with real bat colonies
- LoRa-based mesh networking for multi-node deployments
- Solar charging for indefinite unattended operation
- SD card logging for offline data collection
- Model retraining pipeline for different habitats and species

CavitySense democratizes wildlife monitoring with edge AI -- no cloud required, just a low-cost sensor and the power of the Arduino UNO Q.
