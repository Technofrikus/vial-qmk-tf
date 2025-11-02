#pragma once

#define VIAL_KEYBOARD_UID {0x2A, 0x25, 0x0E, 0xA7, 0x84, 0xF2, 0xF2, 0x06}

#define DYNAMIC_KEYMAP_LAYER_COUNT 6
#define COMBO_ONLY_FROM_LAYER 0

// Memory optimization: Reduce VIAL feature limits to save RAM
// These limits are for the maximum number of entries, not active entries
#define VIAL_COMBO_ENTRIES 16          // Reduce from 32 to 16 (you have 12 active)
#define VIAL_TAP_DANCE_ENTRIES 8       // Reduce from 32 to 8 (you have 2 active)
#define VIAL_KEY_OVERRIDE_ENTRIES 8    // Reduce from 32 to 8 (you have 1 active)
#define DYNAMIC_KEYMAP_MACRO_COUNT 8   // Reduce macro count

// Optimize combo processing to reduce CPU load
#define COMBO_TERM 40                  // Slightly faster combo timeout (default: 50ms)
#define COMBO_STRICT_TIMER             // More strict timing = less processing overhead

// Tap Dance optimization
#define TAPPING_TERM 200               // Standard tapping term

// Reduce dynamic keymap memory footprint
#define DYNAMIC_KEYMAP_MACRO_EEPROM_SIZE 512  // Reduce from 1024 to 512 bytes

// Additional performance optimizations for tap-hold keys
#define PERMISSIVE_HOLD                // Makes tap-hold more responsive
#define HOLD_ON_OTHER_KEY_PRESS        // Faster response for layer switching
#define TAP_CODE_DELAY 10              // Delay between tap code events

