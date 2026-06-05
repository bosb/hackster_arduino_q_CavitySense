# Integration Test Results

Date:
Firmware version (git hash):
Tester:

## Test Conditions

- Environment: [quiet room / outdoor / near tree]
- Distance from mic to sound source: [cm]
- Background noise level: [low / medium / high]

## Classification Results

| Test Sound | Expected | Actual | Confidence | Pass/Fail |
|------------|----------|--------|------------|-----------|
| Silence    | silence  |        |            |           |
| Wind (blow) | wind   |        |            |           |
| Finger snap| wildlife |        |            |           |
| Keys jingle| wildlife |        |            |           |
| Paper crinkle | wildlife |        |        |           |
| Speech     | silence  |        |            |           |

## LED Matrix Verification

- [ ] Startup graph animation plays on boot
- [ ] Idle icon (ring) shows when silent
- [ ] Bat icon animates on wildlife detection
- [ ] Wind icon shows on wind noise
- [ ] Status bar heartbeat pulses on cols 11-12
- [ ] Error icon shows if I2S init fails

## Bridge IPC Verification

- [ ] Events appear on Serial1 as JSON
- [ ] Heartbeat every 2 seconds
- [ ] Bridge.notify sends acoustic_event
- [ ] JSON format correct: `{"v":1,"event":"wildlife","confidence":0.82,"ts":12345}`

## Power / Stability

- [ ] Runs 60 minutes without crash
- [ ] No audio buffer overruns
- [ ] LED matrix remains responsive
- [ ] Classification continues after 1000+ frames
