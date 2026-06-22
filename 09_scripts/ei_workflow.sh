#!/usr/bin/env bash
# ei_workflow.sh — Edge Impulse CLI workflow for CavitySense
#
# Usage:
#   bash 09_scripts/ei_workflow.sh prep     # resample raw audio → 16 kHz mono WAV
#   bash 09_scripts/ei_workflow.sh upload   # upload samples to EI project
#   bash 09_scripts/ei_workflow.sh train    # trigger training via CLI
#   bash 09_scripts/ei_workflow.sh export   # download Arduino library ZIP
#   bash 09_scripts/ei_workflow.sh all      # prep → upload → train → export
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
EI_PROJECT="${EI_PROJECT:-cavitysense-wildlife}"
SAMPLE_DIR="$PROJECT_ROOT/01_ml_pipeline/samples"
RAW_DIR="$PROJECT_ROOT/01_ml_pipeline/raw"

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

step_prep() {
  if ! command -v ffmpeg &>/dev/null; then
    die "ffmpeg not found. Install: brew install ffmpeg"
  fi
  if [ ! -d "$RAW_DIR" ]; then
    warn "No raw/ directory found at $RAW_DIR"
    warn "Place downloaded audio files in subdirectories matching class names:"
    warn "  $RAW_DIR/wildlife_activity/"
    warn "  $RAW_DIR/silence/"
    warn "  $RAW_DIR/wind_noise/"
    exit 1
  fi
  mkdir -p "$SAMPLE_DIR"
  for class_dir in "$RAW_DIR"/*/; do
    class_name="$(basename "$class_dir")"
    out_dir="$SAMPLE_DIR/$class_name"
    mkdir -p "$out_dir"
    count=0
    for f in "$class_dir"/*.wav "$class_dir"/*.mp3 "$class_dir"/*.m4a "$class_dir"/*.flac; do
      [ -f "$f" ] || continue
      count=$((count + 1))
      out_file="$out_dir/${class_name}_$(printf '%03d' "$count").wav"
      info "Converting: $f → $out_file"
      ffmpeg -y -i "$f" -ar 16000 -ac 1 -sample_fmt s16 "$out_file" 2>/dev/null
    done
    success "Processed $count files into $class_name"
  done
}

step_upload() {
  check_cli
  for class_dir in "$SAMPLE_DIR"/*/; do
    [ -d "$class_dir" ] || continue
    class_name="$(basename "$class_dir")"
    wav_count=$(find "$class_dir" -maxdepth 1 -name '*.wav' | wc -l)
    if [ "$wav_count" -eq 0 ]; then
      warn "No WAV files in $class_name — skipping"
      continue
    fi
    info "Uploading $wav_count files for class: $class_name"
    edge-impulse-uploader \
      --project-name "$EI_PROJECT" \
      --category training \
      --label "$class_name" \
      "$class_dir"/*.wav
    success "Uploaded $class_name"
  done
}

step_train() {
  check_cli
  info "Triggering training for project: $EI_PROJECT"
  edge-impulse-run --train --project-name "$EI_PROJECT"
  success "Training triggered. Check status in the Edge Impulse web UI."
}

step_export() {
  check_cli
  info "Downloading Arduino library..."
  edge-impulse-run --download "$PROJECT_ROOT/01_ml_pipeline/"
  for f in "$PROJECT_ROOT"/01_ml_pipeline/*_inferencing.zip; do
    if [ -f "$f" ]; then
      success "Exported: $f"
      echo ""
      echo "  Install with:"
      echo "    arduino-cli lib install --zip-path $f"
    fi
  done
}

case "${1:-help}" in
  prep)    step_prep ;;
  upload)  step_upload ;;
  train)   step_train ;;
  export)  step_export ;;
  all)     step_prep && step_upload && step_train && step_export ;;
  help|*)
    echo "Usage: bash 09_scripts/ei_workflow.sh [prep|upload|train|export|all]"
    echo ""
    echo "Steps:"
    echo "  prep     Resample raw/ audio to 16 kHz mono WAV in samples/"
    echo "  upload   Upload all samples/ to Edge Impulse project"
    echo "  train    Trigger model training"
    echo "  export   Download Arduino library ZIP"
    echo "  all      Run all steps in order"
    ;;
esac
