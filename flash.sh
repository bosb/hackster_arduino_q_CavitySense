#!/usr/bin/env bash
# flash.sh — CavitySense firmware deployment
# Compile + flash Arduino UNO Q MCU firmware via USB.
#
# Usage:
#   ./flash.sh                    # compile + flash main firmware
#   ./flash.sh --monitor          # compile + flash + open serial monitor
#   ./flash.sh --sketch <path>    # flash a specific sketch
#   ./flash.sh --help             # show help

set -euo pipefail

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; BOLD='\033[1m'; NC='\033[0m'
info()    { echo -e "${CYAN}▶${NC} $*"; }
success() { echo -e "${GREEN}✓${NC} $*"; }
warn()    { echo -e "${YELLOW}!${NC} $*"; }
die()     { echo -e "${RED}✗${NC} $*" >&2; exit 1; }

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# ── Default config ──────────────────────────────────────────────
FQBN="arduino:zephyr:unoq:wait_linux_boot=no"
SKETCH_DIR="05_firmware_integration/cavitysense"
MONITOR=false
PORT="auto"

# ── Parse args ──────────────────────────────────────────────────
while [[ $# -gt 0 ]]; do
  case "$1" in
    --monitor|-m) MONITOR=true; shift ;;
    --sketch|-s)  SKETCH_DIR="$2"; shift 2 ;;
    --port|-p)    PORT="$2"; shift 2 ;;
    --help|-h)    echo "Usage: ./flash.sh [--monitor] [--sketch <dir>] [--port <port>]"; exit 0 ;;
    *)            die "Unknown option: $1" ;;
  esac
done

# ── Prerequisites ────────────────────────────────────────────────
command -v arduino-cli >/dev/null 2>&1 || die "arduino-cli not found. Run: bash 09_scripts/setup.sh"

# ── Resolve port ─────────────────────────────────────────────────
if [ "$PORT" = "auto" ]; then
  PORT=$(ls /dev/cu.usbmodem* 2>/dev/null | head -1 || true)
  [ -n "$PORT" ] || die "No USB device found. Plug in Arduino UNO Q."
fi
info "Port: $PORT"

# ── Compile ──────────────────────────────────────────────────────
BUILD_PATH="/tmp/arduino-build-cavitysense"
mkdir -p "$BUILD_PATH"

info "Compiling: $SKETCH_DIR"
info "FQBN: $FQBN"
arduino-cli compile \
  --fqbn "$FQBN" \
  --build-path "$BUILD_PATH" \
  --clean \
  "$SCRIPT_DIR/$SKETCH_DIR" || die "Compilation failed"
success "Compilation OK"

# ── Upload via remoteocd ─────────────────────────────────────────
info "Uploading firmware..."
SERIAL_NO=$(arduino-cli board list --format json 2>/dev/null \
  | python3 -c "
import json,sys
for p in json.load(sys.stdin).get('detected_ports', []):
    if p['port']['address'] == '$PORT':
        print(p['port'].get('properties', {}).get('serialNumber', ''))
        break
" 2>/dev/null || true)

[ -n "$SERIAL_NO" ] || die "Could not read USB serial number."

REMOTEOCD=$(ls ~/Library/Arduino15/packages/arduino/tools/remoteocd/*/remoteocd 2>/dev/null | head -1)
[ -n "$REMOTEOCD" ] || die "remoteocd not found. Run: bash 09_scripts/setup.sh"

ADB_PATH=$(dirname "$(ls ~/Library/Arduino15/packages/arduino/tools/adb/*/adb 2>/dev/null | head -1)")
VARIANT_DIR="$HOME/Library/Arduino15/packages/arduino/hardware/zephyr/0.55.0/variants/arduino_uno_q_stm32u585xx"
FIRMWARE="$BUILD_PATH/sketch.ino.elf-zsk.bin"

[ -f "$FIRMWARE" ] || die "Compiled firmware not found: $FIRMWARE"

"$REMOTEOCD" upload \
  --adb-path "$ADB_PATH/adb" \
  -s "$SERIAL_NO" \
  -f "$VARIANT_DIR/flash_sketch.cfg" \
  "$FIRMWARE" || die "Upload failed"

success "Firmware uploaded successfully"

if [ "$MONITOR" = true ]; then
  info "Opening serial monitor (Ctrl+C to exit)..."
  arduino-cli monitor --port "$PORT" --config "baudrate=115200"
fi
