# 09 — Scripts: Developer Tooling

## Purpose

Shell scripts for one-time setup, firmware flashing, Edge Impulse workflow, and deployment. Enables the fast edit → compile → flash → test cycle.

## Dependencies

- **None** — but scripts assume the directory structure of this project
- `setup.sh` must be run before any other script
- Edge Impulse CLI (optional, for `ei_workflow.sh`)

## Files to Create

| File | Role |
|------|------|
| `setup.sh` | One-time environment setup: install arduino-cli, board packages, dependencies |
| `deploy.env` | Local config: serial port, FQBN, device IP (gitignored) |
| `deploy.env.template` | Template with placeholder values |
| `ei_workflow.sh` | Edge Impulse CLI workflow: upload data, train, export |
| `flash_sketch_fixed.cfg` | OpenOCD flash config for UNO Q (fixed variant — writes magic word before reset) |

## Step-by-Step Implementation

### 1. `setup.sh`

```bash
#!/usr/bin/env bash
# One-time developer environment setup
# Usage: bash 09_scripts/setup.sh

set -euo pipefail

# 1. Install arduino-cli (macOS via Homebrew)
if ! command -v arduino-cli &>/dev/null; then
  brew install arduino-cli
fi

# 2. Update core index
arduino-cli core update-index

# 3. Install Zephyr board core (for UNO Q)
arduino-cli core install arduino:zephyr

# 4. Install required libraries
arduino-cli lib install "Arduino_LED_Matrix"
arduino-cli lib install "ArduinoFFT"

# 5. Copy deploy.env template if not exists
if [ ! -f deploy.env ]; then
  cp 09_scripts/deploy.env.template deploy.env
  echo ">>> EDIT deploy.env with your board's serial port"
fi

echo "Setup complete. Verify with: arduino-cli board list"
```

### 2. `deploy.env.template`

```bash
# CavitySense deploy config
# Copy to deploy.env and fill in (deploy.env is gitignored)

# USB serial port (auto-detect with: ls /dev/cu.usbmodem*)
ARDUINO_PORT=auto

# Board FQBN
ARDUINO_FQBN=arduino:zephyr:unoq:wait_linux_boot=no

# Linux side IP (if using dashboard deployment)
DEVICE_IP=192.168.1.100
DEVICE_USER=arduino

# Edge Impulse project name
EI_PROJECT=cavitysense-wildlife
```

### 3. `ei_workflow.sh`

```bash
#!/usr/bin/env bash
# Edge Impulse workflow helper
# Steps: upload data → train → export Arduino library
# Prerequisites: edge-impulse-cli (npm install -g edge-impulse-cli)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
EI_PROJECT="${EI_PROJECT:-cavitysense-wildlife}"

step_upload() {
  echo "Uploading samples to Edge Impulse..."
  edge-impulse-uploader \
    --project-name "$EI_PROJECT" \
    --category training \
    "$PROJECT_ROOT/01_ml_pipeline/samples/"*.wav
}

step_export() {
  echo "Exporting Arduino library..."
  edge-impulse-run --download "$PROJECT_ROOT/01_ml_pipeline/"
}

case "${1:-help}" in
  upload)   step_upload ;;
  export)   step_export ;;
  all)      step_upload && step_export ;;
  help|*)   echo "Usage: bash 09_scripts/ei_workflow.sh [upload|export|all]" ;;
esac
```

### 4. `flash_sketch_fixed.cfg`

OpenOCD config adapted for the UNO Q STM32U5. This file disables the sketch wait-for-Linux-boot mechanism so the firmware starts immediately after flash.

```text
# flash_sketch_fixed.cfg — OpenOCD config for Arduino UNO Q (STM32U585)
# Modified: write magic word while CPU is halted (before final reset)
# so the Zephyr kernel boots directly into the new sketch.

reset_config srst_only srst_nogate srst_push_pull connect_assert_srst
init
reset
halt
flash info 0
flash write_image erase ${filename} 0x8100000 bin
mww 0x40036400 0xCAFFEEEE
reset
shutdown
```

## Expected Outputs

- `setup.sh` — run once to prepare environment
- `deploy.env` and `deploy.env.template` — config
- `ei_workflow.sh` — Edge Impulse automation
- `flash_sketch_fixed.cfg` — OpenOCD flash config

## Verification

- [ ] `bash 09_scripts/setup.sh` completes without errors
- [ ] `arduino-cli board list` detects UNO Q when plugged in
- [ ] `./flash.sh` compiles and flashes successfully
- [ ] Serial monitor shows firmware output after flash
