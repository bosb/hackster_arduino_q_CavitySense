#!/usr/bin/env bash
# ei_workflow.sh — Edge Impulse workflow helper for CavitySense
# Steps: upload data → train → export Arduino library
# Prerequisites: edge-impulse-cli (npm install -g edge-impulse-cli)
#
# Usage: bash 09_scripts/ei_workflow.sh [upload|export|all]

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
EI_PROJECT="${EI_PROJECT:-cavitysense-wildlife}"
SAMPLE_DIR="$PROJECT_ROOT/01_ml_pipeline/samples"

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
CYAN='\033[0;36m'; NC='\033[0m'
info()    { echo -e "${CYAN}▶${NC} $*"; }
success() { echo -e "${GREEN}✓${NC} $*"; }
warn()    { echo -e "${YELLOW}!${NC} $*"; }
die()     { echo -e "${RED}✗${NC} $*" >&2; exit 1; }

check_cli() {
  command -v edge-impulse-uploader >/dev/null 2>&1 || \
    die "edge-impulse-uploader not found. Install: npm install -g edge-impulse-cli"
}

step_upload() {
  check_cli
  info "Uploading samples to Edge Impulse project: $EI_PROJECT"
  if [ ! -d "$SAMPLE_DIR" ]; then
    die "Sample directory not found: $SAMPLE_DIR"
  fi
  wav_count=$(find "$SAMPLE_DIR" -name '*.wav' | wc -l)
  if [ "$wav_count" -eq 0 ]; then
    die "No WAV files found in $SAMPLE_DIR"
  fi
  info "Found $wav_count WAV files"
  edge-impulse-uploader \
    --project-name "$EI_PROJECT" \
    --category training \
    "$SAMPLE_DIR"/*.wav
  success "Upload complete"
}

step_export() {
  check_cli
  info "Exporting Arduino library..."
  edge-impulse-run --download "$PROJECT_ROOT/01_ml_pipeline/"
  # Rename to expected path
  for f in "$PROJECT_ROOT"/01_ml_pipeline/*_inferencing.zip; do
    if [ -f "$f" ]; then
      success "Exported: $f"
      info "Install with: arduino-cli lib install --zip-path $f"
    fi
  done
}

case "${1:-help}" in
  upload)   step_upload ;;
  export)   step_export ;;
  all)      step_upload && step_export ;;
  help|*)   echo "Usage: bash 09_scripts/ei_workflow.sh [upload|export|all]" ;;
esac
