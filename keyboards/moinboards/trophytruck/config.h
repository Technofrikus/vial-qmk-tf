// Copyright 22 ScatteredDrifter / fabiansta

#pragma once

#include "config_common.h"

/* --- USB Device properties --- */
#define VENDOR_ID 0xFAB0
#define PRODUCT_ID 0xFAB3
#define DEVICE_VER 0x0001
#define MANUFACTURER Moinboards
#define PRODUCT Trophytruck

/* --- Matrix definition --- */
#define MATRIX_ROWS 4
#define MATRIX_COLS 12

/* --- Matrix assignment --- */ 
#define MATRIX_COL_PINS \
    { GP18, GP9, GP8, GP7, GP6, GP5, GP4, GP3, GP2, GP1, GP25, GP28 }
#define MATRIX_ROW_PINS \
    { GP19, GP20, GP0, GP15 } 

/* --- additional settings --- */
#define DEBUG_MATRIX_SCAN_RATE
#define LAYER_STATE_16BIT
// reducing chattering, set to 0 if not necessary 
#define DEBOUNCE 5 
#define DIODE_DIRECTION COL2ROW
	
/* --- VIAL  --- */

#define VIAL_KEYBOARD_UID {0x0E, 0x44, 0xE2, 0x4A, 0x6E, 0xCB, 0x53, 0xD2}

#define VIAL_UNLOCK_COMBO_ROWS { 1, 1 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 1 }

//encoders
#define ENCODERS_PAD_A { GP24, GP27 }
#define ENCODERS_PAD_B { GP26, GP29 }	
	
#define DYNAMIC_KEYMAP_LAYER_COUNT 6
	
//#define ENCODER_RESOLUTION 100
//#define ENCODER_DEFAULT_POS 0x3
	

/* --- rgb settings --- */
//#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
//#define RGB_MATRIX_KEYPRESSES
//#define RGB_DI_PIN GP8
//#define RGBLED_NUM 0
//#define NOP_FUDGE 0.5

//#define QMK_WAITING_TEST_BUSY_PIN GP8
//#define QMK_WAITING_TEST_YIELD_PIN GP9

/* --- RP2040 specific settings */
//#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
//#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP25
//#define RP2040_BOOTLOADR_DOUBLE_TAP_RESET_TIMEOUT 500U

