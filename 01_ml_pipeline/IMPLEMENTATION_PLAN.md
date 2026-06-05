# 01 — ML Pipeline: Edge Impulse TinyML Audio Classifier

## Purpose

Train a lightweight 3-class audio classifier using Edge Impulse that runs on the Arduino UNO Q MCU (STM32). The model classifies short audio frames into: `silence`, `wind_noise`, or `wildlife_activity`.

## Dependencies

- **None** — this can be implemented independently, on any machine with a browser
- **Output artifact**: Edge Impulse exported Arduino library ZIP → consumed by `04_firmware_classifier`

## Files to Create

| File | Role |
|------|------|
| `DATA_COLLECTION.md` | Guide for recording WAV samples for each class |
| `EDGE_IMPULSE_SETUP.md` | Step-by-step Edge Impulse project setup + impulse config |
| `model_config.json` | Exact model architecture and training parameters |
| `TRAINING_LOG.md` | Record of training results, accuracy, confusion matrix |

## Step-by-Step Implementation

### 1. Create Edge Impulse Project

1. Go to [edgeimpulse.com](https://edgeimpulse.com), sign up/log in
2. Create new project → name: `cavitysense-wildlife`
3. Select **Audio Classification**
4. Target device: **Generic Arduino (Cortex-M)** — exact board support not needed

### 2. Collect Training Data (Record WAV Files)

Do **not** use live streaming. Record WAVs offline and upload.

| Class | What to Record | Target Duration |
|-------|----------------|-----------------|
| `silence` | Quiet room, night ambient, forest background | 5-10 min |
| `wind_noise` | Blow across mic, fan noise, paper rustling | 5-10 min |
| `wildlife_activity` | Finger snaps, key jingling, high-frequency tapping, short squeaks, paper crinkling | 5-10 min |

Recording specs:
- Format: 16-bit WAV
- Sample rate: **16 kHz** (must match firmware)
- Mono
- Record in short 10-30 second clips
- Vary distance to mic and intensity

### 3. Configure Impulse

| Parameter | Value |
|-----------|-------|
| Window size | **1000 ms** |
| Window increase | **500 ms** |
| Processing block | **MFCC** (Spectral Analysis) |
| Learning block | **Classification (Neural Network)** |

### 4. MFCC Settings

| Setting | Value |
|---------|-------|
| Frame length | 25 ms |
| Frame stride | 10 ms |
| Number of coefficients | 13 |
| FFT length | 512 |
| Low frequency | 300 Hz |
| High frequency | 8000 Hz |
| Noise floor | -52 dB |

### 5. Neural Network Architecture

```text
Input (MFCC features)
→ Dense(16, relu)
→ Dropout(0.25)
→ Dense(8, relu)
→ Dropout(0.25)
→ Dense(3, softmax)   # silence, wind_noise, wildlife_activity
```

Why this size: The UNO Q MCU (STM32) has ~256 KB flash, ~128 KB RAM. Keeping the model under ~20 KB ensures room for audio buffers and stack.

Training params:
- Epochs: 50
- Learning rate: 0.0005
- Batch size: 32
- Data augmentation: add noise

### 6. Train & Validate

1. Click **Generate features** after configuring MFCC
2. Review feature explorer — classes should separate visually
3. Train model — target accuracy > 80%
4. Run model testing on held-out data
5. Review confusion matrix

### 7. Export

1. Enable **INT8 quantization**
2. Go to **Deployment** → select **Arduino library**
3. Click **Build** → download ZIP
4. Save to: `01_ml_pipeline/cavitysense-wildlife_inferencing.zip`

## Expected Outputs

- `cavitysense-wildlife_inferencing.zip` — Edge Impulse exported Arduino library (consumed by `04_firmware_classifier`)
- `model_config.json` — record of settings
- `TRAINING_LOG.md` — accuracy, confusion matrix screenshots
- `samples/` — collection of WAV files used for training

## Verification

- [ ] Edge Impulse project created with 3 classes
- [ ] At least 30 total WAV samples uploaded
- [ ] Impulse configured with MFCC + NN blocks
- [ ] Training accuracy > 80%
- [ ] INT8 quantization enabled
- [ ] Arduino library ZIP exported successfully
- [ ] Model size under 20 KB
