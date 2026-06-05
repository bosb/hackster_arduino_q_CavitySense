# Project Description — CavitySense

## Title

CavitySense: AI-Powered Wildlife Monitoring for Tree Conservation

## Scenario

Social Impact — AI-Powered Wildlife Monitoring System

## What Are You Going to Build?

CavitySense is a low-cost, AI-powered acoustic monitoring node designed to detect wildlife activity inside tree cavities before tree removal or infrastructure development. The system captures environmental audio using a digital MEMS microphone and analyzes it locally on the Arduino UNO Q to estimate the probability of wildlife presence. Visual feedback is provided through the onboard LED matrix, and activity data is transmitted to a mobile dashboard for logging and review. The goal is to provide a scalable pre-screening tool that supports data-driven conservation decisions.

## How Does Your Solution Work?

CavitySense operates as an edge AI device. System Workflow:

1. The INMP441 microphone captures environmental sound at 16 kHz
2. The Arduino UNO Q processes audio frames in real time (1024 samples, 64ms windows)
3. FFT-based spectral features are extracted from the signal
4. A lightweight Edge Impulse TinyML classifier determines the probability of wildlife activity across 3 classes: silence, wind_noise, wildlife_activity
5. Results are displayed locally via the 8×13 grayscale LED matrix and transmitted to a connected dashboard via App Lab

Main Features:
- Real-time acoustic monitoring
- On-device AI inference (edge computing, no cloud needed)
- Wildlife activity probability scoring
- LED-based visual status indicator (bat icon animates on detection)
- Mobile dashboard logging and visualization via App Lab
- Low-cost, deployable hardware architecture

## Sustainability

CavitySense supports sustainability in multiple ways:
- Preventative environmental screening: helps identify inhabited trees before removal
- Low energy consumption: edge processing eliminates continuous cloud computation
- Minimal hardware footprint: single microcontroller + digital microphone
- Scalable monitoring: enables targeted professional ecological assessment only where activity is detected

## User Experience

The device is designed for non-technical users:
- On-site: LED matrix shows monitoring status, bat animation on detection
- Remote: App Lab dashboard displays activity score, classification result, timestamped history
- No specialist training required for initial deployment

## Scalability

- Multi-node deployment across forested areas
- Aggregated activity heatmaps
- Integration with GIS-based planning systems
- Adaptation for other cavity-dwelling species monitoring
- Same architecture can be applied to agricultural biodiversity monitoring, urban wildlife observation, or environmental impact studies

## App Lab Integration

App Lab serves as the mobile and web-based visualization layer. It displays real-time activity scores, classification results, and historical activity data without requiring custom backend infrastructure.

## AI Integration

AI is integrated through a lightweight Edge Impulse TinyML model running directly on the Arduino UNO Q MCU. Audio is processed into spectral features and classified in real time on-device — no raw audio is sent to the cloud.
