// Copyright 2023 LazyDesigners (@LazyDesigners)
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

//safe some space
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE

#define WS2812_DI_PIN B3
//Enable for normal RGB lighting
// #define RGBLED_NUM 4
//Enable for Matrix RGB lighting
#define RGB_MATRIX_LED_COUNT 4

//RGB effects (non-Matrix)
//#define RGBLIGHT_EFFECT_BREATHING
//#define RGBLIGHT_EFFECT_RAINBOW_MOOD   
//#define RGBLIGHT_EFFECT_RAINBOW_SWIRL
// #define RGBLIGHT_EFFECT_SNAKE
/* #define RGBLIGHT_EFFECT_CHRISTMAS       */
//#define RGBLIGHT_EFFECT_STATIC_GRADIENT  
/* #define RGBLIGHT_EFFECT_KNIGHT           */
/* #define RGBLIGHT_EFFECT_RGB_TEST         */
/* #define RGBLIGHT_EFFECT_ALTERNATING      */ 
/*#define RGBLIGHT_EFFECT_TWINKLE          */ 


//Normal Matrix effects
#define ENABLE_RGB_MATRIX_BREATHING
#define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_CYCLE_ALL
#define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS

//Dynamic Matric effects
/* 
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define RGB_MATRIX_KEYPRESSES
#define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
#define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH
*/