#pragma once

#include "config_common.h"

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 15
// ROWS: Top to bottom, COLS: Left to right
#define MATRIX_ROW_PINS {D0,D1,D2,D3,D5}
#define MATRIX_COL_PINS {D7,D6,D4,B4,B5,B6,C6,C7,F7,F6,F5,F4,F1,F0,E6}

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* VIAL */
#define VIAL_KEYBOARD_UID {0x1c, 0x6B, 0xDC, 0xF8, 0x63, 0x56, 0xEA, 0x9A}
#define VIAL_UNLOCK_COMBO_ROWS {0,1} 
#define VIAL_UNLOCK_COMBO_COLS {0,2}
#define VIAL_KEY_OVERRIDE_ENTRIES 10
#define VIAL_TAP_DANCE_ENTRIES 5
#define VIAL_COMBO_ENTRIES 6

/* define if matrix has ghost */
//#define MATRIX_HAS_GHOST

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5

#define QMK_ESC_OUTPUT  D7 // usually COL
#define QMK_ESC_INPUT   D0 // usually ROW

