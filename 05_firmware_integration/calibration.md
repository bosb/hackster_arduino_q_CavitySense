# CavitySense Calibration Guide

## Threshold Tuning

The confidence threshold determines when a classification is "real" vs ignored:

| Threshold | Behavior |
|-----------|----------|
| 0.9 | Only very confident detections trigger alerts (few false positives, may miss real events) |
| 0.75 (default) | Good balance for most environments |
| 0.6 | More sensitive (may trigger on borderline sounds) |
| 0.5 | Very sensitive (many false positives) |

To change: edit `CONFIDENCE_THRESHOLD` in `04_firmware_classifier/src/cavitysense_classifier.h`

## Calibration Procedure

1. **Silence baseline**: Place device in quiet environment. Observe that classification stays at "silence" with confidence < 0.5
2. **Wind threshold**: Gently blow across mic. Should trigger "wind_noise" at >0.75 confidence
3. **Wildlife simulation**: Snap fingers or jingle keys 30cm from mic. Should trigger "wildlife_activity" at >0.75 confidence
4. **Mixed sounds**: Talk near the device while making wildlife-like sounds. Model should still pick wildlife over speech

## Feature Scaling

If features seem too low or too high, adjust normalization in `feature_extraction.cpp`:

```
// Feature values should be 0.0–1.0 after normalization
// If all values are very small (< 0.01), check FFT magnitude scaling
// If all values are saturated (all near 1.0), adjust FFT windowing
```

## Audio Gain

The INMP441 has fixed sensitivity. If audio is too quiet:

1. Ensure the mic is within 1m of the sound source
2. Check that I2S 32-bit frames are being read correctly (upper 24 bits)
3. The ADC reference is 3.3V — signals should use the full dynamic range

## Serial Debug Output

Monitor with `./flash.sh --monitor` to see:

```
peak=12345 features: 0.0012 0.0034 0.0456 0.1234 0.2345 0.3456 0.4567 0.5678
Result: silence (0.2%)
peak=23456 features: 0.0023 0.0056 0.0789 0.2345 0.4567 0.6789 0.7890 0.8901
Result: wildlife_activity (82.3%)
```
