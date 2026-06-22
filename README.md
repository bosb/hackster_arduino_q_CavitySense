# 🦇 CavitySense: AI-Powered Wildlife Monitoring for Tree Conservation

Low-cost acoustic monitoring node that detects wildlife activity inside tree cavities using Arduino UNO Q + ELV MEMS1 analog MEMS microphone + Edge Impulse TinyML + App Lab dashboard.

## Project Structure

| Module | Purpose |
|--------|---------|
| `01_ml_pipeline/` | Edge Impulse TinyML training (3-class audio classifier) |
| `02_firmware_audio/` | ELV MEMS1 analog capture, FFT, feature extraction library |
| `03_firmware_led_matrix/` | 13×8 grayscale LED matrix graphics (bat icons, status bar) |
| `04_firmware_classifier/` | Edge Impulse inference + label→icon mapping library |
| `05_firmware_integration/` | Combined firmware sketch (audio + classifier + LED + Bridge IPC) |
| `06_app_lab_dashboard/` | Python Flask dashboard deployed via App Lab |
| `07_hardware/` | Wiring diagrams, BOM, enclosure |
| `08_documentation/` | Hackster contest submission materials |
| `09_scripts/` | Dev tooling: setup, deploy, Edge Impulse workflow |

## Quick Start

```bash
# One-time setup
bash 09_scripts/setup.sh

# Full deploy cycle (firmware + app)
./flash.sh

# Or step by step
./flash.sh --monitor        # compile, flash, open serial monitor
make flash-audio-test       # flash just the audio capture test
make flash-led-test         # flash just the LED matrix test
```

## Board

**Arduino UNO Q** — FQBN: `arduino:zephyr:unoq:wait_linux_boot=no`
