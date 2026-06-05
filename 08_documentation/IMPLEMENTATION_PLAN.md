# 08 — Documentation: Hackster Contest Submission

## Purpose

All materials needed for the Hackster "Invent the Future with Arduino UNO Q and App Lab" contest submission. This module is purely documentation — no code.

## Dependencies

- All other modules must be implemented first (hardware verified, firmware working, dashboard deployed)
- Photos of the working device
- Video of the system in action

## Files to Create

| File | Role |
|------|------|
| `project_description.md` | Polished submission text (from ChatGPT conversation draft) |
| `system_architecture.md` | Architecture diagram description |
| `video_script.md` | 60-second pitch script |
| `bom.md` | Bill of materials for submission page |
| `judging_notes.md` | Key selling points aligned to each judging criterion |
| `photos/` | Photos of the build (empty dir, add images separately) |
| `how_it_works.md` | Technical explanation for non-technical judges |

## Step-by-Step Implementation

### 1. Project Description (`project_description.md`)

Use the text already drafted in the ChatGPT conversation. Key sections:

- **Title**: CavitySense: AI-Powered Wildlife Monitoring for Tree Conservation
- **Scenario**: Social Impact — AI-Powered Wildlife Monitoring System
- **What you built**: 2-3 paragraph overview
- **How it works**: Technical summary
- **Main features**: Bullet list
- **Sustainability**: Environmental impact framing
- **User experience**: How to deploy and use
- **Scalability**: Multi-node deployment vision

### 2. System Architecture (`system_architecture.md`)

Block diagram description (already drafted in conversation):

```
[Tree Cavity Sound]
↓
[INMP441 Digital Microphone]  →  I2S (16 kHz)
↓
[Arduino UNO Q MCU (STM32)]
├── FFT Feature Extraction
├── Edge Impulse TinyML Inference
├── LED Matrix Visualization (13×8)
└── Bridge IPC (Serial1 JSON)
↓
[Arduino UNO Q Linux side]
├── Flask Web Dashboard
└── WiFi AP → Browser
↓
[App Lab / Mobile Dashboard]
```

### 3. Video Script (`video_script.md`)

60-second pitch:

| Time | Visual | Audio |
|------|--------|-------|
| 0-10s | Device on desk, LED matrix idle | "CavitySense is a low-cost AI-powered wildlife monitor..." |
| 10-20s | Hand approaches, snaps fingers | "...that detects acoustic activity inside tree cavities..." |
| 20-30s | LED bat animation activates | "...using an INMP441 microphone and Edge Impulse TinyML." |
| 30-40s | Phone showing dashboard | "Results appear on a mobile dashboard via App Lab..." |
| 40-50s | Photo of tree with device | "...helping conservation teams identify which trees to protect." |
| 50-60s | All components visible | "CavitySense: democratizing wildlife monitoring with edge AI." |

### 4. Judging Notes (`judging_notes.md`)

Map the project to each judging criterion:

| Criterion | How CavitySense Addresses It |
|-----------|------------------------------|
| **Social Impact** | Pre-screening tool for tree removal decisions; protects bat habitats during infrastructure projects |
| **AI Integration** | Edge Impulse TinyML classifier running on-device; FFT feature extraction; real-time inference |
| **Innovation** | Low-cost alternative to professional ultrasonic detectors; conservation-specific application |
| **User Experience** | LED matrix status display; mobile dashboard; no specialist training required |
| **Scalability** | Multi-node deployment; same architecture works for other cavity-dwelling species |
| **Sustainability** | Edge processing (no cloud); minimal hardware; reduces unnecessary tree removal |

### 5. Photos (`photos/`)

Capture:

1. **Breadboard prototype** — INMP441 wired to UNO Q
2. **LED matrix close-up** — bat icon illuminated
3. **Dashboard screenshot** — App Lab dashboard showing activity
4. **Full setup** — device + phone showing dashboard
5. **Context shot** — device near tree (if possible)

### 6. How It Works (`how_it_works.md`)

Plain-language explanation:

- The INMP441 microphone listens to sounds near a tree cavity
- The Arduino UNO Q converts these sounds into frequency patterns
- A TinyML model (trained with Edge Impulse) classifies the patterns
- Results flash on the LED matrix and send to a dashboard
- Conservation teams check which trees have high activity

## Expected Outputs

- All markdown files listed above
- `photos/` directory (images added manually)
- Ready-to-copy text for Hackster submission form

## Verification

- [ ] Project description complete and proofread
- [ ] Architecture diagram described clearly
- [ ] Video script timed to 60 seconds
- [ ] Photos taken and stored in `photos/`
- [ ] Judging notes cover all 5 criteria
- [ ] BOM matches actual hardware used
- [ ] Submission text pasted into Hackster form
