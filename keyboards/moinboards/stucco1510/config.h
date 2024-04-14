// Copyright 22 ScatteredDrifter / fabiansta

#pragma once

#include "config_common.h"

/* --- USB Device properties --- */
#define VENDOR_ID 0xFAB0
#define PRODUCT_ID 0xFAB3
#define DEVICE_VER 0x0001
#define MANUFACTURER Moinboards
#define PRODUCT Stucco1510

/* --- Matrix definition --- */
#define MATRIX_ROWS 5 
#define MATRIX_COLS 15

/* --- Matrix assignment --- */ 
#define MATRIX_COL_PINS \
    { GP25, GP24, GP23, GP22, GP21, GP20, GP19, GP18, GP17, GP16, GP15, GP14, GP12, GP11, GP10 }
#define MATRIX_ROW_PINS \
    { GP5, GP4, GP7, GP9, GP13 } 

/* --- Encoder assignment --- */
#define ENCODER_RESOLUTION 4
#define ENCODERS_PAD_A {GP0, GP2}
#define ENCODERS_PAD_B {GP1, GP3}
/* --- additional settings --- */
#define DEBUG_MATRIX_SCAN_RATE
#define LAYER_STATE_16BIT
#define DYNAMIC_KEYMAP_LAYER_COUNT 10
// reducing chattering, set to 0 if not necessary 
#define DEBOUNCE 5 
#define DIODE_DIRECTION COL2ROW

/* --- rgb settings --- */
//#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
//#define RGB_MATRIX_KEYPRESSES
#define RGB_DI_PIN GP8
//#define RGBLED_NUM 0
//#define NOP_FUDGE 0.5

//#define QMK_WAITING_TEST_BUSY_PIN GP8
//#define QMK_WAITING_TEST_YIELD_PIN GP9

/* --- RP2040 specific settings */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
//#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP25
#define RP2040_BOOTLOADR_DOUBLE_TAP_RESET_TIMEOUT 500U

