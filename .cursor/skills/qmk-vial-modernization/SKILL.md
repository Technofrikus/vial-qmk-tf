---
name: qmk-vial-modernization
description: Guidelines for updating QMK/Vial keyboards to modern standards (keyboard.json, separated config.h). Use this when dealing with compilation errors related to deprecated info.json or top-level VIAL_KEYBOARD_UID.
---

# QMK and Vial Modernization

This skill provides the workflow for updating older QMK/Vial keyboard configurations to the modern standard.

## When to Use

- When building older keyboards results in linting errors like `VIAL_KEYBOARD_UID in config.h is no longer a valid option`.
- When QMK tools suggest using `keyboard.json` instead of `info.json`.
- When moving hardware configuration (RGB, OLED) to JSON.

## Instructions

### 1. File Naming
- Rename `info.json` to `keyboard.json` in the keyboard root.

### 2. Vial Configuration
- **Move Vial Macros**: Move `VIAL_KEYBOARD_UID`, `VIAL_UNLOCK_COMBO_ROWS`, and `VIAL_UNLOCK_COMBO_COLS` from the top-level `config.h` to the keymap-specific `config.h` (e.g., `keymaps/vial/config.h`).
- **UID Format**: Ensure `VIAL_KEYBOARD_UID` is a comma-separated hex array: `#define VIAL_KEYBOARD_UID {0x.., 0x..}`.

### 3. Hardware Configuration (JSON)
- Move RGB and OLED settings into `keyboard.json` to avoid linting warnings and centralize hardware info.
- **RGB Matrix/Light**:
  ```json
  "rgblight": {
      "led_count": 10,
      "animations": { "all": true }
  },
  "ws2812": { "pin": "B5" }
  ```
- **OLED**: Ensure `OLED_DRIVER = ssd1306` (lowercase) in `rules.mk` and `"oled": true` in `keyboard.json` features.

### 4. Compilation Fixes & Size Optimization
- **Firmware too large**: 
  - Enable LTO: `LTO_ENABLE = yes` in `rules.mk`.
  - Disable QMK Settings: `QMK_SETTINGS = no` in `rules.mk` (if using Vial).
- **EEPROM Limit**: If `Dynamic keymaps are configured to use more EEPROM than is available` error occurs:
  - Reduce `DYNAMIC_KEYMAP_LAYER_COUNT` in `config.h` (e.g., from 8 to 4).
  - Reduce `VIAL_COMBO_ENTRIES` or `VIAL_TAP_DANCE_ENTRIES`.

## Best Practices
- Always clean before flashing if strange errors occur: `make <keyboard>:<keymap>:clean`.
- Use `make <keyboard>:<keymap>` without `:flash` to test compilation first.
- Refer to `keyboards/0_privat/bally` as a modern reference structure.
