# How It Works — Plain Language

## The Problem

When trees need to be cut down for construction projects, authorities must first check if protected animals like bats live in them. Professional bat detectors are expensive and require experts to operate.

## Our Solution

CavitySense is a small, cheap device that listens to tree cavities and detects whether animals are active inside.

## How It Works

1. **Listening**: A tiny microphone (INMP441) picks up sounds near a tree cavity
2. **Analyzing**: The Arduino UNO Q converts sounds into frequency patterns (like a musical note chart)
3. **AI Classification**: A trained AI model (Edge Impulse) classifies the patterns into three categories:
   - *Silence* — no activity
   - *Wind noise* — environmental noise, not animals
   - *Wildlife activity* — sounds that could be bats, birds, or small mammals
4. **Showing Results**: The built-in LED matrix displays a bat icon when wildlife is detected
5. **Reporting**: The device sends results to a mobile dashboard that conservation teams can check remotely

## Why It's Useful

- Costs much less than professional wildlife monitoring equipment
- Anyone can deploy it — no special training needed
- It helps identify which trees actually need protection, reducing unnecessary delays in construction projects
- Multiple devices can monitor an entire forest, creating a map of wildlife activity
