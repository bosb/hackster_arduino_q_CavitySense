# Edge Impulse Setup Guide

## Project Summary

| Item | Value |
|------|-------|
| Platform | Edge Impulse |
| Project name | `cavitysense-wildlife` |
| Target device | Arduino UNO Q |
| Microcontroller | STM32 |
| Audio input | INMP441 microphone |
| Sample rate | 16 kHz mono |
| Classes | `silence`, `wind_noise`, `wildlife_activity` |

## A. Create Project

1. Go to [edgeimpulse.com](https://edgeimpulse.com) and create account
2. Click **Create new project** → name: `cavitysense-wildlife`
3. Select **Audio Classification**

## B. Collect Training Data

Record WAV files on your laptop/phone and upload. Do NOT use live streaming.

| Class | What to Record | Target |
|-------|---------------|--------|
| `silence` | Quiet room, night ambient, outdoor forest background | 5-10 min |
| `wind_noise` | Blow gently across mic, fan noise, paper rustling | 5-10 min |
| `wildlife_activity` | Finger snaps, key jingling, high-frequency tapping, paper crinkling, short squeaks | 5-10 min |

### Recording specs:
- 16-bit WAV, **16 kHz**, mono
- 10-30 second clips
- Vary distance to mic and intensity
- ~30 total samples minimum

### Upload:
1. Go to **Data acquisition** tab
2. Click **Upload data** → select WAV files
3. Label each file with the correct class
4. Split data: 80% training, 20% testing

## C. Configure Impulse

| Parameter | Value |
|-----------|-------|
| Window size | 1000 ms |
| Window increase | 500 ms |
| Processing block | MFCC |
| Learning block | Classification (Neural Network) |

### MFCC Settings

| Setting | Value |
|---------|-------|
| Frame length | 25 ms |
| Frame stride | 10 ms |
| Num coefficients | 13 |
| FFT length | 512 |
| Low frequency | 300 Hz |
| High frequency | 8000 Hz |
| Noise floor | -52 dB |

## D. Neural Network Architecture

```text
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

## E. Train, Validate, Export

1. Click **Generate features**
2. Review feature explorer — classes should separate
3. Click **Start training**
4. Target accuracy: >80%
5. Enable **INT8 quantization**
6. Go to **Deployment** → **Arduino library** → **Build**
7. Download ZIP, save as `cavitysense-wildlife_inferencing.zip`

## Expected Model Size

- Quantized INT8: ~10-20 KB
- RAM usage: ~20-30 KB
- Well within UNO Q limits (128 KB RAM, 256 KB Flash)

## Verification Checklist

- [ ] 3 classes created: silence, wind_noise, wildlife_activity
- [ ] At least 30 WAV samples uploaded
- [ ] MFCC + NN impulse configured
- [ ] Training accuracy >80%
- [ ] INT8 quantization enabled
- [ ] Arduino library ZIP exported
- [ ] Model size < 20 KB
