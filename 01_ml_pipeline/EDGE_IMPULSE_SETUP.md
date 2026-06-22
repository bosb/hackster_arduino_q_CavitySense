# Edge Impulse Setup Guide (CLI-first)

## Overview

This guide uses Edge Impulse CLI tools exclusively — no web UI required for day-to-day operations. You only need the web UI once to create the project and get an API key.

### Prerequisites

```bash
# Install Edge Impulse CLI
npm install -g edge-impulse-cli

# Verify
edge-impulse-uploader --version
edge-impulse-daemon --version
edge-impulse-run --version
```

### One-time web setup

1. Go to <https://edgeimpulse.com> and create an account
2. Create a new project named `cavitysense-wildlife`
3. Choose **Audio Classification**
4. Go to **Dashboard** → **Keys** → copy your **API key**
5. Export it for CLI use:

```bash
export EI_API_KEY="ei_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
export EI_PROJECT_ID="xxxxx"
```

---

## A. Data Collection

Follow `01_ml_pipeline/DATA_COLLECTION.md` to prepare audio samples.

Expected structure:

```
01_ml_pipeline/samples/
  silence/
    silence_001.wav
    silence_002.wav
    ...
  wind_noise/
    wind_001.wav
    ...
  wildlife_activity/
    bat_001.wav
    snaps_001.wav
    ...
```

---

## B. Upload Data (CLI)

```bash
# Upload each class with the correct label
edge-impulse-uploader \
  --project-name cavitysense-wildlife \
  --category training \
  --label silence \
  01_ml_pipeline/samples/silence/*.wav

edge-impulse-uploader \
  --project-name cavitysense-wildlife \
  --category training \
  --label wind_noise \
  01_ml_pipeline/samples/wind_noise/*.wav

edge-impulse-uploader \
  --project-name cavitysense-wildlife \
  --category training \
  --label wildlife_activity \
  01_ml_pipeline/samples/wildlife_activity/*.wav
```

Alternatively, use `edge-impulse-daemon` in data-forwarder mode for live labeling.

### Split data (web UI required once)

1. Go to your project in the browser
2. **Data acquisition** → click **Perform train/test split**
3. 80% training, 20% testing

---

## C. Configure Impulse

These settings are configured in the web UI under **Impulse design**:

### MFCC (Spectral Analysis)

| Setting | Value |
|---------|-------|
| Window size | 1000 ms |
| Window increase | 500 ms |
| Frame length | 25 ms |
| Frame stride | 10 ms |
| Number of coefficients | 13 |
| FFT length | 512 |
| Low frequency | 300 Hz |
| High frequency | 8000 Hz |
| Noise floor | -52 dB |

### Neural Network

```
Input (MFCC features)
→ Dense(16, relu)
→ Dropout(0.25)
→ Dense(8, relu)
→ Dropout(0.25)
→ Dense(3, softmax)
```

### Training settings

| Setting | Value |
|---------|-------|
| Epochs | 50 |
| Learning rate | 0.0005 |
| Batch size | 32 |
| Data augmentation | Add noise |
| Target accuracy | > 80% |

---

## D. Train & Export (CLI)

### Train model

```bash
# Trigger training via CLI
edge-impulse-run --train --project-name cavitysense-wildlife
```

Wait for training to complete. Check status in the web UI. Target: > 80% accuracy on the test set.

### Export Arduino library

```bash
# Download the Arduino library ZIP
edge-impulse-run --download 01_ml_pipeline/
```

This saves `cavitysense-wildlife_inferencing.zip` to `01_ml_pipeline/`.

### Install the library

```bash
arduino-cli lib install --zip-path \
  01_ml_pipeline/cavitysense-wildlife_inferencing.zip
```

---

## E. Verify

| Check | Expected |
|-------|----------|
| Training accuracy | > 80% |
| INT8 quantization | Enabled |
| Model size | < 20 KB |
| Inference time per frame | < 64 ms |
| ZIP installed as Arduino library | Compiles in `inference_test.ino` |

---

## Troubleshooting

| Problem | Fix |
|---------|-----|
| `edge-impulse-uploader` not found | Run `npm install -g edge-impulse-cli` |
| Upload fails with auth error | Check `EI_API_KEY` is set correctly |
| Low accuracy | Add more varied training data; check class balance |
| Model too large | Reduce NN layers; use fewer MFCC coefficients |

---

## References

- <https://docs.edgeimpulse.com/docs/tools/edge-impulse-cli>
- <https://docs.edgeimpulse.com/docs/edge-impulse-run>
- <https://www.avosound.com/de/geraeusche/tiere/fledermaus/>
- <https://www.fledermausschutz-suedhessen.de/praktisches/fledermausrufe.htm>
