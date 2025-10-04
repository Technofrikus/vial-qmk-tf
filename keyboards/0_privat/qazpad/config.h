// Copyright 2023 NoahK (@KiserDesigns)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

// VIAL_KEYBOARD_UID is now defined in info.json, but potentially still needed here for older vial.c code
#define VIAL_KEYBOARD_UID {0xCF, 0x69, 0x44, 0x95, 0x1E, 0x7A, 0xA8, 0x2E}

//#define VIA_EEPROM_LAYOUT_OPTIONS_SIZE 2

#define VIAL_TAP_DANCE_ENTRIES 6

#define VIAL_COMBO_ENTRIES 15
#define COMBO_TERM 30
#define COMBO_ONLY_FROM_LAYER 0

#define DYNAMIC_KEYMAP_LAYER_COUNT 5

#define VIAL_KEY_OVERRIDE_ENTRIES 3

// Unlock combo is now handled by Vial automatically or configured elsewhere if needed
// #define VIAL_UNLOCK_COMBO_ROWS { 0, 4 }
// #define VIAL_UNLOCK_COMBO_COLS { 0, 0 }

#define LAYER_STATE_8BIT


#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY

// RGB Lighting Configuration - Hardware defined in info.json
// #define WS2812_DI_PIN B3
// #define RGBLIGHT_LED_COUNT 45

// RGBLIGHT Default Settings
#define RGBLIGHT_EFFECT_BREATHING // Also need to explicitly enable the effect we're setting as default
// #define RGBLIGHT_MODE_BREATHING // Default effect - Temporarily disabled for testing
#define RGBLIGHT_HUE_STEP 8     // Default hue step for controls
#define RGBLIGHT_SAT_STEP 8     // Default saturation step for controls
#define RGBLIGHT_VAL_STEP 8     // Default brightness step for controls
#define RGBLIGHT_LIMIT_VAL 150  // Default maximum brightness (0-255)
#define RGBLIGHT_SLEEP          // Turn off LEDs when computer sleeps


// Effects should be enabled in rules.mk
// #define RGBLIGHT_EFFECT_RAINBOW_MOOD
// #define RGBLIGHT_MODE_KNIGHT

