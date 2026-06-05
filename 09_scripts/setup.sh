#!/usr/bin/env bash
# setup.sh — One-time developer environment setup for CavitySense
# Usage: bash 09_scripts/setup.sh

set -euo pipefail

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; NC='\033[0m'
info()    { echo -e "${CYAN}[setup]${NC} $*"; }
success() { echo -e "${GREEN}[✓]${NC} $*"; }
warn()    { echo -e "${YELLOW}[!]${NC} $*"; }
die()     { echo -e "${RED}[✗]${NC} $*" >&2; exit 1; }

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo -e "${CYAN}══ CavitySense Developer Setup ══${NC}"

# 1. Homebrew
if ! command -v brew &>/dev/null; then
  die "Homebrew not found. Install it first: https://brew.sh"
fi
success "Homebrew found"

# 2. arduino-cli
if ! command -v arduino-cli &>/dev/null; then
  info "Installing arduino-cli..."
  brew install arduino-cli
else
  success "arduino-cli: $(arduino-cli version | head -1)"
fi

# 3. Board core (Zephyr for UNO Q)
info "Updating core index..."
arduino-cli core update-index

if arduino-cli core list | grep -q "arduino:zephyr"; then
  success "Board core already installed: arduino:zephyr"
else
  info "Installing Arduino Zephyr board core..."
  arduino-cli core install arduino:zephyr
fi

# 4. Required libraries
info "Installing libraries..."
arduino-cli lib install "Arduino_LED_Matrix" 2>/dev/null || warn "Arduino_LED_Matrix may not be installable via CLI — check Arduino IDE"
arduino-cli lib install "ArduinoFFT" 2>/dev/null || warn "ArduinoFFT may not be installable via CLI — check Arduino IDE"

# 5. deploy.env
if [ ! -f "$PROJECT_ROOT/deploy.env" ]; then
  cp "$SCRIPT_DIR/deploy.env.template" "$PROJECT_ROOT/deploy.env"
  warn "Created deploy.env — edit with your device info"
fi

# 6. .gitignore
if ! grep -q "deploy.env" "$PROJECT_ROOT/.gitignore" 2>/dev/null; then
  echo "deploy.env" >> "$PROJECT_ROOT/.gitignore"
  success "Added deploy.env to .gitignore"
fi

echo
echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${GREEN}Setup complete.${NC}"
echo
echo "Next steps:"
echo "  1. Wire INMP441 per 07_hardware/WIRING.md"
echo "  2. Plug in UNO Q via USB"
echo "  3. Run: arduino-cli board list  (verify board detected)"
echo "  4. Train model: follow 01_ml_pipeline/EDGE_IMPULSE_SETUP.md"
echo "  5. Flash: ./flash.sh --monitor"
