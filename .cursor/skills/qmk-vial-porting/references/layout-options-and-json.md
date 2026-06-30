# Layout options and vial.json — extended reference

## How layout options map to the UI

From [Vial — Create JSON](https://get.vial.today/docs/porting-to-via.html):

1. Define `layouts.labels` — order sets **option index** (0, 1, 2, …).
2. In KLE, put `optionIndex,choiceIndex` in the **bottom-right** legend of each variant key.
3. Put alternate variants on **extra rows** in KLE (often below the main board), with spacing (`x`, `y`, `w`) so bounding boxes line up.
4. Boolean option: choice `0` = off, `1` = on.
5. Choice option: choice `0` = first label after caption, `1` = second, etc.

## KLE → vial.json string encoding

| KLE legend | vial.json key string |
|------------|----------------------|
| Top-left only `1,0` | `"1,0"` |
| Matrix `3,5`, option 1 choice 0 | `"3,5\n\n\n1,0"` |
| Encoder CCW (index 0) | `"0,0\n\n\n\n\n\n\n\n\ne"` (see encoder doc) |

When merging KLE raw JSON into `vial.json`, wrap decorative keys in objects:

```json
{ "w": 6.25 },
"3,5\n\n\n1,0"
```

## Worked pattern: boolean + bottom-row choice

**Labels:**

```json
"labels": [
  "LAE",
  ["Bottom Row", "6.25u", "3u+3u", "2.25u+2.75u"]
]
```

**Option 0 (LAE):** same matrix keys, different visuals

- `0,9\n\n\n0,0` — wide backspace (choice 0)
- `0,9\n\n\n0,1` — alternate (choice 1), placed with `x` offset in KLE
- `1,9\n\n\n0,0` vs `1,9\n\n\n0,1` — enter variants

**Option 1 (bottom row):** separate KLE rows

| Choice | Keys (matrix in top-left) |
|--------|---------------------------|
| 0 — 6.25u | `3,0\n\n\n1,0`, `3,1\n\n\n1,0`, `w:6.25` + `3,5\n\n\n1,0` |
| 1 — 3u+3u | `3,0\n\n\n1,1`, `3,1\n\n\n1,1`, `3,4\n\n\n1,1`, `3,7\n\n\n1,1` |
| 2 — 2.25+2.75 | `3,0`, `3,1\n\n\n1,2`, `3,2\n\n\n1,2`, `3,4\n\n\n1,2`, `3,7\n\n\n1,2` |

Right-thumb cluster (`3,9`, `3,10`, `3,11`) often stays on its own row with no option suffix — always visible.

## Aligning `keyboard.json` with KLE default

`keyboard.json` should reflect the **default** option set (usually all choice `0`):

1. List keys in `LAYOUT` macro order (same as `keymap.c`).
2. Use `w` / `h` from the default KLE row (e.g. `6.25` on space).
3. Include every matrix position once — including keys only used in non-default bottom rows (`3,2`, `3,4`, `3,7`) with correct coordinates.

`keymap.c` example for unused alternates:

```c
KC_LCTL, KC_LGUI, KC_SPC, KC_LALT, KC_RGUI, KC_RSFT,
KC_NO,   KC_NO,   KC_NO    // 3,4 3,7 3,2 — used when user picks 3u/2.25 layouts
```

## Deriving matrix positions from QMK `LAYOUT`

If `keymap.c` has:

```c
#define LAYOUT(k00, k01, ..., k39) { \
    { k00, k01, ... }, \
    ...
}
```

Walk the macro left-to-right, top-to-bottom in the first argument list, then map each `kXY` to `[row,col]` using the second half of the macro (or `keyboard.json` layout array).

## Stripping VIA JSON

When adapting from [the-via/keyboards](https://github.com/the-via/keyboards):

- Remove VIA-specific metadata Vial rejects.
- Keep `layouts.keymap` KLE structure and `labels`.
- Re-test with dummy JSON.

## Encoder block in `vial.json` (minimal)

Two encoders, no click:

```json
"keymap": [
  [
    {"y": 0.25, "x": 0.5},
    "0,0\n\n\n\n\n\n\n\n\ne",
    "0,1\n\n\n\n\n\n\n\n\ne"
  ],
  ["0,0", "0,1", "0,2"]
]
```

Encoder index increments per encoder: `1,0` / `1,1` … for second encoder.

## Lighting strings (quick)

```json
"lighting": "none"
"lighting": "qmk_backlight"
"lighting": "qmk_rgblight"
"lighting": "qmk_backlight_rgblight"
"lighting": "vialrgb"
```

Match firmware: no `VIALRGB_ENABLE` → do not use `"vialrgb"`.

## References in this repo

- `keyboards/kiserdesigns/suffer/v2/keymaps/vial/vial.json` — multiple choice options
- `keyboards/jlw/lestmovoz/keymaps/vial/vial.json` — `\n\n\n` on space keys
- `keyboards/idb/idb_60/keymaps/vial/vial.json` — classic multi-option (upstream vial-qmk)
