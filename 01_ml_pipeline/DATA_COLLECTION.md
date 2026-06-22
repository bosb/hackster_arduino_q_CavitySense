# Data Collection Guide

## Overview

Three classes: `silence`, `wind_noise`, `wildlife_activity`. At least 10 minutes per class.

**No audio files are stored in this repo.** Only the URLs and preparation commands are tracked.

---

## Audio Sources

### Wildlife activity — bat calls (primary)

| Source | URL | Notes |
|--------|-----|-------|
| Avosound bat collection | <https://www.avosound.com/de/geraeusche/tiere/fledermaus/> | Large library of European bat species echolocation and social calls |
| Fledermausschutz Südhessen | <https://www.fledermausschutz-suedhessen.de/praktisches/fledermausrufe.htm> | Recordings of multiple bat species with spectrograms |

Download a diverse selection — different species, call types (social, feeding buzz, search-phase), and environments.

### Wildlife activity — synthetic sounds (supplementary)

| Source | Description |
|--------|-------------|
| DIY | Finger snaps, key jingling, paper crinkling, tapping on wood, short squeaks, tongue clicks |
| Record yourself at 16 kHz mono WAV, 10-30 second clips |

### Silence

Record quiet indoor and outdoor ambient. No talking, no music.

### Wind noise

Blow gently across the mic, move a sheet of paper near it, or record near a fan on low speed.

---

## Prerequisites

- `ffmpeg` (install via `brew install ffmpeg` or `apt install ffmpeg`)
- At least ~500 MB free disk for downloaded + processed audio

---

## Workflow

### 1. Download raw audio (outside repo)

```bash
# Create a download directory OUTSIDE the repo
mkdir -p ~/cavitysense_raw/bat_calls
```

Download bat call WAV/MP3 files from the URLs above into `~/cavitysense_raw/bat_calls/`. Do not check these into the project.

### 2. Prepare samples

```bash
# Create output directories (gitignored)
mkdir -p 01_ml_pipeline/samples/silence
mkdir -p 01_ml_pipeline/samples/wind_noise
mkdir -p 01_ml_pipeline/samples/wildlife_activity

# Convert and label bat calls
# Name convention: label_name.wav or the script infers from directory
for f in ~/cavitysense_raw/bat_calls/*.wav; do
  ffmpeg -y -i "$f" \
    -ar 16000 -ac 1 -sample_fmt s16 \
    01_ml_pipeline/samples/wildlife_activity/$(basename "$f")
done

# Record silence / wind noise directly into the right directory
# (Record using Audacity, sox, or the UNO Q itself)
```

### 3. Verify sample rate

```bash
ffprobe 01_ml_pipeline/samples/wildlife_activity/example.wav \
  2>&1 | grep -E 'Audio:|Hz'
```

Every file must be: **16-bit PCM, 16 kHz, mono**.

---

## Data Budget

| Class | Target total | Min clips | Source |
|-------|-------------|-----------|--------|
| `silence` | 10 min | 20 | DIY record |
| `wind_noise` | 10 min | 20 | DIY record |
| `wildlife_activity` | 10 min | 30 | Bat URLs + DIY supplementary |

More variety > more quantity. Include some recordings with mixed sounds (wind + faint wildlife).

---

## Licensing

- Bat call recordings from public databases may be CC-licensed or research-use only
- **Do not redistribute the downloaded audio files**
- This repo only tracks the URLs and preparation commands
- DIY recordings are your own work and free to use
- If publishing the project (Hackster, GitHub), note that the model was trained using publicly available bat call datasets and DIY recordings; do not rehost the original audio files
