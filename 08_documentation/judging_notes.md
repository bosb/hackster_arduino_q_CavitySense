# Judging Notes

## Social Impact

- Addresses real conservation problem: identifying wildlife-inhabited trees before infrastructure projects (railway lines, housing developments)
- Low-cost design (~€10 in parts) makes it accessible to community groups and small municipalities
- Inspired by actual news article about bat habitat protection during railway electrification in Germany

## AI Integration

- Uses Edge Impulse TinyML — real ML pipeline with dataset, training, INT8 quantization, and on-device inference
- FFT-based feature extraction feeds a neural network classifier running on the STM32 MCU
- 3-class audio classification (silence, wind_noise, wildlife_activity) with configurable confidence threshold
- All inference at the edge — no cloud, no latency, privacy-preserving

## Innovation

- Novel application: conservation pre-screening tool rather than just a detector
- Combines affordable MEMS microphone with TinyML for a use case normally requiring expensive ultrasonic equipment
- LED matrix provides immediate on-site visual feedback without needing a phone
- Dual-core architecture leverages both MCU and Linux sides of the UNO Q

## User Experience

- Plug-and-play: power on, LED matrix shows status immediately
- No specialist training required — green dot = monitoring, bat animation = wildlife detected
- App Lab dashboard accessible from any browser on the local network
- Historical logging allows trend analysis over days or weeks

## Scalability

- Multi-node deployment: same hardware can be replicated across many trees
- Centralized dashboard aggregates data from all nodes
- Retrainable model: same hardware can adapt to different environments or species
- Modular architecture allows future enhancements (LoRa, SD logging, solar power)
