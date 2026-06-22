# AGENTS.md — CavitySense

## Build / Lint / Test Commands

### Firmware (Arduino UNO Q, FQBN: `arduino:zephyr:unoq:wait_linux_boot=no`)

| Command | What it does |
|---------|-------------|
| `arduino-cli compile --fqbn "arduino:zephyr:unoq:wait_linux_boot=no" --clean 05_firmware_integration/cavitysense/` | Compile main sketch |
| `arduino-cli compile --fqbn "arduino:zephyr:unoq:wait_linux_boot=no" --clean 02_firmware_audio/examples/audio_test/` | Compile audio test |
| `arduino-cli compile --fqbn "arduino:zephyr:unoq:wait_linux_boot=no" --clean 03_firmware_led_matrix/examples/led_test/` | Compile LED test |
| `arduino-cli compile --fqbn "arduino:zephyr:unoq:wait_linux_boot=no" --clean 04_firmware_classifier/examples/inference_test/` | Compile inference test |
| `./flash.sh` | Compile + flash main firmware |
| `./flash.sh --monitor` | Compile + flash + open serial monitor (115200 baud) |
| `./flash.sh --sketch <dir>` | Flash a specific sketch directory |
| `make flash-audio-test` | Flash audio capture test |
| `make flash-led-test` | Flash LED matrix test |
| `make flash-ml-test` | Flash classifier inference test |
| `make clean` | Remove build artifacts in /tmp |
| `arduino-cli monitor --port <port> --config "baudrate=115200"` | Open serial monitor |

### Dashboard (Python Flask)

| Command | What it does |
|---------|-------------|
| `python3 06_app_lab_dashboard/src/main.py` | Run dashboard on port 7000 |
| `pip install -r 06_app_lab_dashboard/requirements.txt` | Install deps |

### Dev Scripts

| Command | What it does |
|---------|-------------|
| `bash 09_scripts/setup.sh` | One-time dev environment setup |
| `make setup` | Same as above |

### Notes

- Arduino CLI must be installed (`brew install arduino-cli`)
- Upload uses `remoteocd` (not standard `arduino-cli upload`), see `flash.sh` for details
- OpenOCD config: `$VARIANT_DIR/flash_sketch.cfg` (stock) or `09_scripts/flash_sketch_fixed.cfg` (modified magic-word timing)
- Install required libraries: `arduino-cli lib install "ArduinoFFT"` (the header is `arduinoFFT.h`, lowercase)
- `Arduino_LED_Matrix` is built into the `arduino:zephyr` core (0.1.3)
- There is no test framework — verification is done by compiling + flashing to hardware

---

## Code Style Guidelines

### Language: C++ (Arduino firmware)

**Includes**
- Arduino/library headers: `#include <Arduino_LED_Matrix.h>`
- Project headers in same directory: `#include "audio_capture.h"`
- Sort: system/library first, then project headers

**Naming**
- Functions/variables: `snake_case` (e.g., `audio_init`, `display_idle`)
- Types/structs/enums: `PascalCase` (e.g., `ClassifierResult`, `AcousticEvent`)
- Module-level `static` state: prefix with `_` (e.g., `_ready`, `_buffer_pos`)
- Macros/constants: `UPPER_SNAKE_CASE` (e.g., `AUDIO_SAMPLE_RATE`, `PIN_I2S_WS`)
- API functions: `module_action_qualifier()` pattern (e.g., `audio_get_frame()`, `display_bat_animate()`)

**Headers**
- Use traditional `#ifndef`/`#define`/`#endif` include guards
- Include `<Arduino.h>` for all Arduino types
- Keep API declarations minimal, with inline `//` comments per function

**Formatting**
- Indent: **2 spaces** (no tabs)
- Braces: Egyptian style (opening brace on same line)
- No trailing whitespace
- Section dividers: `// ── Section Name ─────────────────────`

**Constants & State**
- `#define` for pins, timing, sizes (e.g., `#define BRIDGE_INTERVAL_MS 2000u`)
- Module-level static variables for file-private state
- Float literals use `f` suffix (e.g., `0.0f`, `0.75f`)

**Error handling**
- Return `bool` or `nullptr` on failure; log via `Serial.print` or `#pragma message`
- Edge Impulse library: use `__has_include` to detect and compile in bypass mode if missing
- Error state in main loop: flash icon and halt (`while (1) { display_with_status(ICON_ERROR); delay(500); }`)

**Comments**
- Minimal; explain *why*, not *what*
- Hardware pin assignments documented inline
- Frequency bands documented with table comments
- Suppress Edge Impulse logging with stub `ei_printf()` / `ei_printf_float()`

**LLEXT libc shims** (required on UNO Q Zephyr)
```cpp
extern "C" int* __errno(void) {
  static int shim_errno = 0;
  return &shim_errno;
}
```

### Language: Python (Flask dashboard)

- Indent: **4 spaces**
- Imports: stdlib first, then third-party (Flask)
- Naming: `snake_case` for functions/vars, `PascalCase` for classes
- String quotes: double `""` consistently
- Flask patterns: route decorators, `render_template`, `jsonify`, SSE via `Response(event_stream(), mimetype="text/event-stream")`
- `if __name__ == "__main__":` guard at bottom

### Language: JavaScript (Frontend)

- Indent: **2 spaces**
- Naming: `camelCase` for functions/vars
- String quotes: single `''` consistently
- DOM access: `document.getElementById()`
- EventSource SSE for real-time updates
- Arrow functions for callbacks

### Language: CSS

- Indent: **2 spaces**
- Dark theme: `#0a1a0f` bg, `#7cff7c` green accent, `#ffcc44` amber
- Naming: `kebab-case` for class names
- Mobile-first responsive at `max-width: 400px`

### Language: Shell (bash scripts)

- `set -euo pipefail` at top
- Helper functions: `info()`, `success()`, `warn()` (stdout), `die()` (stderr + exit 1)
- Color variables: `RED`, `GREEN`, `YELLOW`, `CYAN`, `BOLD`, `NC`
- Project root derived via `SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"`

### General

- No generated files, build artifacts, or credentials committed (`deploy.env` in `.gitignore`)
- Edge Impulse model exports (`*_inferencing.zip`) in `.gitignore`
- LED matrix layout: 13 cols × 8 rows = 104 bytes; cols 10-12 reserved for status bar
- I2S pins: D7=L/R, D8=SD, D9=SCK, D10=WS (custom mapping, not default)
