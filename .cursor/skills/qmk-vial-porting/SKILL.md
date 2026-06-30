---
name: qmk-vial-porting
description: Port keyboards to Vial firmware—vial.json (KLE, layout options), keyboard.json LAYOUT, keymaps, encoders, and lighting. Use when creating a new Vial port, fixing Vial UI layout, or building keymaps/vial/vial.json. Pair with qmk-vial-modernization for migrating old QMK trees.
---

# QMK / Vial keyboard porting

End-to-end workflow for **new** Vial ports in `vial-qmk`. For updating deprecated `info.json` / top-level `VIAL_KEYBOARD_UID`, use **qmk-vial-modernization** first, then this skill.

Official docs:
- [Create keyboard JSON (KLE, layout options)](https://get.vial.today/docs/porting-to-via.html)
- [Port to Vial (keymap, UID, compile)](https://get.vial.today/docs/porting-to-vial.html)
- [Encoders](https://get.vial.today/docs/encoders.html)
- [Lighting](https://get.vial.today/docs/lighting.html)

## Two different JSON files

| File | Location | Purpose |
|------|----------|---------|
| `keyboard.json` | Keyboard root | QMK firmware: MCU, matrix pins, `LAYOUT` macro, features, bootmagic |
| `vial.json` | `keymaps/vial/vial.json` | Vial app only: visual layout, layout options, lighting mode string |

**Never** rename or copy `info.json` / `keyboard.json` → `vial.json`. Formats differ.

`matrix.rows` / `matrix.cols` in **vial.json** must equal the **physical** switch matrix (`matrix_pins` row/col count), not the number of KLE rows or layout-option rows.

## Hardware vs `vial.json` conflicts (mandatory)

**Stop and ask the user before changing firmware or JSON** when anything disagrees, for example:

- User-provided (or schematic) row/column **pin lists** vs `vial.json` `matrix.rows` / `matrix.cols`
- Any top-left `row,col` legend in `vial.json` outside `0..rows-1` / `0..cols-1`
- Switch count in `layouts.LAYOUT.layout` vs the obvious pin count

**Do not** invent extra matrix pins, delete pins, or remap matrix coordinates to force a compile without explicit user confirmation. After the user clarifies, update **both** `keyboard.json` and `vial.json` to the same physical matrix.

## Porting checklist

### A. QMK keyboard works

1. `make <path>:default` succeeds before Vial work.
2. Matrix pins and `diode_direction` correct in `keyboard.json`.
3. `layouts.LAYOUT.layout` lists **every physical switch once**, with real `[row, col]` matrix positions.

### B. `vial.json` (UI definition)

1. Build layout in [KLE](http://www.keyboard-layout-editor.com/); **top-left** legend = matrix `row,col` (zero-based, row first).
2. Add **layout options** via **bottom-right** legends (see below).
3. Raw data → Download JSON → wrap in template (below).
4. Set `vendorId` / `productId` to match `keyboard.json` `usb.vid` / `usb.pid`.
5. **Verify before compile:** Vial app → File → Load dummy JSON. Fix errors and test every layout-option dropdown.

### C. Vial keymap

1. Copy `keymaps/default` → `keymaps/vial` (keep `default`).
2. `keymaps/vial/rules.mk`:
   ```make
   VIA_ENABLE = yes
   VIAL_ENABLE = yes
   ```
   Optional features at keymap level only, e.g. `ENCODER_MAP_ENABLE = yes`, `VIALRGB_ENABLE = yes`.
3. `keymaps/vial/config.h`: `VIAL_KEYBOARD_UID`, unlock combo (or `VIAL_INSECURE = yes` for private builds only).
   ```bash
   python3 util/vial_generate_keyboard_uid.py
   ```
4. `keymap.c`: `LAYOUT(...)` argument order **must match** `keyboard.json` `layouts.LAYOUT.layout` order exactly.
5. Map **all** matrix positions used on the PCB; layout options do not add switches—use `KC_NO` for keys only used in alternate bottom rows until the user assigns them in Vial.
6. `DYNAMIC_KEYMAP_LAYER_COUNT` in keyboard or keymap `config.h` must match layer count in `keymap.c`.

### D. Build

```bash
make <keyboard_path>:vial
make <keyboard_path>:vial:clean   # if stale errors
```

Reference tree: `keyboards/0_privat/bally`, `keyboards/vial_example/`.

## `vial.json` template

```json
{
  "name": "MyKeyboard",
  "vendorId": "0xXXXX",
  "productId": "0xXXXX",
  "lighting": "none",
  "matrix": {
    "rows": 7,
    "cols": 12
  },
  "layouts": {
    "labels": [
      "OptionalBoolOption",
      ["Choice Option Name", "Choice A", "Choice B", "Choice C"]
    ],
    "keymap": []
  }
}
```

- Omit `labels` entirely if there are no layout options.
- `lighting`: see [Lighting](#lighting-vialjson) below.
- Paste KLE `keymap` array into `layouts.keymap`.

## Layout options (critical)

Documented in [porting-to-via](https://get.vial.today/docs/porting-to-via.html).

### Labels array

- **Boolean:** a single string → `"Split Backspace"`.
- **Choice:** `["Caption", "opt0", "opt1", ...]` — first string is UI label; choices are index `0`, `1`, `2`, …

Example:

```json
"labels": [
  "LAE",
  ["Bottom Row", "6.25u", "3u+3u", "2.25u+2.75u"]
]
```

- Option index `0` = `LAE` (off/on).
- Option index `1` = bottom row (choices 0–2).

### KLE bottom-right legend

Format: `optionIndex,choiceIndex` (not matrix coordinates).

- `0,0` / `0,1` on the **same** matrix key (e.g. `0,9`) = two visual variants for boolean option 0.
- `1,0` / `1,1` / `1,2` = bottom-row **choices** for option 1.

### Export string in `vial.json`

KLE encodes bottom-right legends after newlines on the key string:

```json
"0,9\n\n\n0,0"
```

Meaning: matrix position `0,9`, layout option `0` choice `0`.

### Extra `keymap` rows

Alternate bottom rows / split keys appear as **additional arrays** in `layouts.keymap` (separate KLE rows), often with `y` / `w` offsets. They are **not** extra matrix rows.

### Common mistake

Keys like `"4,0"` in an alternate bottom-row KLE row are often read as matrix row 4—but the PCB may only have rows `0–6`. In that case `4,0` in the **top-left** legend would be wrong; in **bottom-right** it means option 4, choice 0. Always distinguish:

| Legend position | Meaning |
|-----------------|--------|
| Top-left (or sole `"r,c"` string) | Matrix `row,col` |
| Bottom-right (`\n\n\nopt,choice`) | Layout option only |

Wrong `matrix.rows` (e.g. 8×13 when hardware is 7×12) breaks dummy JSON load and confuses porting.

### Firmware vs UI

- **vial.json** controls which keys are shown for each layout option.
- **keymap.c** still defines one keycode per **physical** matrix cell; all variants share the same dynamic keymap entry.

## `keyboard.json` LAYOUT

`layouts.LAYOUT.layout` is an array of objects:

```json
{
  "matrix": [0, 0],
  "x": 0,
  "y": 0,
  "w": 1,
  "h": 1
}
```

Rules:

1. **One object per switch** in the order used by `LAYOUT(...)` in `keymap.c`.
2. `matrix` must exist on the PCB (within `matrix_pins` dimensions).
3. `x` / `y` / `w` / `h` are visual (1u = 1.0); align with default KLE option for sensible Configurator layout.
4. Do **not** duplicate the same `[row,col]` twice in `LAYOUT`.
5. Layout-option-only visuals belong in **vial.json**, not as fake matrix positions in `keyboard.json`.

Bootmagic example:

```json
"bootmagic": {
  "enabled": true,
  "matrix": [0, 0]
}
```

## Encoders

[QMK encoders](https://docs.qmk.fm/#/feature_encoders) at keyboard level:

```json
"features": { "encoder": true },
"encoder": {
  "rotary": [
    { "pin_a": "B0", "pin_b": "C14", "resolution": 4 }
  ]
}
```

Vial keymap `rules.mk`: `ENCODER_MAP_ENABLE = yes` (keymap only).

`keymap.c`:

```c
#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
};
#endif
```

Layer count must match the main keymap. Remove encoder **callbacks** from `keymaps/vial/keymap.c`; keep them in `default` if needed.

### Encoders in `vial.json`

- Not matrix keys: two (or three with click) 1u keys with center legend `e`.
- KLE export example: `"0,0\n\n\n\n\n\n\n\n\ne"` — encoder index and CW/CCW in top legend per [encoder docs](https://get.vial.today/docs/encoders.html).
- Clickable encoder: bottom 1u is normal matrix key; rotary pair is separate.

## Lighting (`vial.json`)

| Hardware | QMK setup | `"lighting"` in vial.json |
|----------|-----------|---------------------------|
| None | — | `"none"` |
| Single-color backlight | `BACKLIGHT_ENABLE`, `BACKLIGHT_BREATHING` | `"qmk_backlight"` |
| RGB underglow | `RGBLIGHT_ENABLE` | `"qmk_rgblight"` |
| Both | both enabled | `"qmk_backlight_rgblight"` |
| Per-key / RGB matrix + Vial control | RGB matrix + keymap `VIALRGB_ENABLE = yes` | `"vialrgb"` |

Optional: `#define VIALRGB_NO_DIRECT` if direct HID control uses too much memory.

RGB config belongs in `keyboard.json` (see qmk-vial-modernization), not in `vial.json`.

## Vial keymap `config.h` (typical)

```c
#pragma once

#define VIAL_KEYBOARD_UID {0x.., 0x.., 0x.., 0x.., 0x.., 0x.., 0x.., 0x..}
#define VIAL_UNLOCK_COMBO_ROWS { 0, 0 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 1 }

#define VIAL_COMBO_ENTRIES 32
#define VIAL_TAP_DANCE_ENTRIES 32
#define VIAL_KEY_OVERRIDE_ENTRIES 16
```

Do not `#include "vial.h"` in `keymap.c` for combo introspection—use `VIAL_ENABLE` and `build_vial.mk`.

## Validation workflow

1. **Dummy JSON** — layout, options, encoders visible.
2. **`make …:vial`** — compiles; watch flash/EEPROM size.
3. **On device** — unlock combo, remap keys, toggle each layout option in Vial and confirm visuals match hardware.

## When stuck

| Symptom | Check |
|---------|--------|
| Pin count ≠ `vial.json` matrix size | **Ask user** — do not guess an extra/missing pin; align JSON after confirmation |
| Dummy JSON fails | `matrix` size, malformed JSON, VIA-only fields stripped |
| Wrong key in Vial vs press | `LAYOUT` order ≠ `keyboard.json` ≠ KLE matrix legends |
| Phantom keys / wrong row count | Layout option legend mistaken for matrix coords |
| Layout option does nothing | Missing `\n\n\nopt,choice` on keys; missing `labels` entry |
| Encoder won't configure | Missing `e` keys in vial.json; `ENCODER_MAP_ENABLE`; layer count |
| No RGB in Vial | `lighting` string + `VIALRGB_ENABLE` / QMK RGB feature |

Extended examples: [references/layout-options-and-json.md](references/layout-options-and-json.md)

Discord: [Vial](https://discord.gg/zNKEUXTKwF)
