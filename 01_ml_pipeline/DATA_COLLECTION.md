# Data Collection Guide

## Recording Equipment

- Laptop built-in microphone, external USB mic, or phone
- Or use the ELV MEMS1 + UNO Q (record via serial, convert to WAV)
- Any device that can record 16-bit/16 kHz WAV files

## Recording Environment

Find a **quiet indoor space** for base recordings. For wildlife-like sounds, use household objects.

## Per-Class Recording Instructions

### silence (10 minutes total)

Record the ambient background of:
- A quiet room (no talking, no music)
- Outdoor night ambient (if possible)
- Near the target tree location

Keep the mic stationary. Avoid sudden noises.

### wind_noise (10 minutes total)

Simulate wind by:
- Blowing gently across the microphone at varying distances (5-30 cm)
- Moving a sheet of paper near the mic
- Recordings near a fan on low speed
- Rustling fabric near the mic

### wildlife_activity (10 minutes total)

Record acoustic events that simulate animal activity in a tree cavity:
- **Finger snaps** — sharp, high-frequency bursts
- **Key jingling** — metallic high-frequency sounds
- **Paper crinkling** — broadband noise with high-frequency content
- **Tapping on wood** — simulate woodpecker/bird activity
- **Short squeaks** — using a squeaky toy or rubber sole on floor
- **Clicking sounds** — tongue clicks, pen clicks

Vary the intensity (soft to loud) and distance (10-100 cm from mic).

## File Format

- 16-bit PCM WAV
- 16 kHz sample rate
- Mono (single channel)
- Keep clips 10-30 seconds each
- Name files like: `silence_01.wav`, `wind_noise_03.wav`, `wildlife_05.wav`

## Upload to Edge Impulse

1. Go to **Data acquisition** → **Upload data**
2. Select all WAV files for one class
3. Label with the class name
4. Auto-split: 80% training / 20% testing
5. Repeat for each class

## Tips for Good Classification

- More variety > more quantity
- Include some recordings with **mixed** sounds (e.g., wind + faint wildlife)
- Add some recordings at the actual deployment location
- If classifier struggles, add more examples of the confusing pair
