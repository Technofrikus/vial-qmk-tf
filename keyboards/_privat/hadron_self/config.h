// Copyright 2023 Technofrikus (@Technofrikus)
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

#define VIAL_KEYBOARD_UID {0x31, 0x45, 0xBA, 0x8A, 0xEC, 0xBC, 0xD1, 0x1C}

#define VIAL_TAP_DANCE_ENTRIES 32

#define VIAL_COMBO_ENTRIES 32

#define DYNAMIC_KEYMAP_LAYER_COUNT 5

#define VIAL_UNLOCK_COMBO_ROWS { 0, 4 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 0 }

// RGB RGB RGB
// The pin connected to the data pin of the LEDs
#define WS2812_DI_PIN B5
// The number of LEDs connected
#define RGBLED_NUM 10

#define RGBLIGHT_EFFECT_BREATHING
#define RGBLIGHT_EFFECT_RAINBOW_MOOD   
#define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#define RGBLIGHT_EFFECT_STATIC_GRADIENT
#define RGBLIGHT_EFFECT_SNAKE
#define RGBLIGHT_EFFECT_ALTERNATING
#define RGBLIGHT_EFFECT_TWINKLE

/*
//AUDIO
#define AUDIO_PIN A5
#define AUDIO_PIN_ALT A4
#define AUDIO_PIN_ALT_AS_NEGATIVE

//#define AUDIO_ENABLE_TONE_MULTIPLEXING
//#define AUDIO_TONE_MULTIPLEXING_RATE_DEFAULT 10ffggfhgfffghhgfh
#ifdef AUDIO_ENABLE

#define AUDIO_CLICKY
  // to enable clicky on startup 
  //#define AUDIO_CLICKY_ON
#define AUDIO_CLICKY_FREQ_RANDOMNESS 1.5f
#endif

*/
/*
//HAPTIC
#define SOLENOID_PIN A14

#define DRV2605L_FB_ERM_LRA 1
#define DRV2605L_FB_BRAKEFACTOR 3 // For 1x:0, 2x:1, 3x:2, 4x:3, 6x:4, 8x:5, 16x:6, Disable Braking:7 
#define DRV2605L_FB_LOOPGAIN 1 // For  Low:0, Medium:1, High:2, Very High:3 

#define RATED_VOLTAGE 2
#define V_RMS 2.0
#define V_PEAK 2.85
#define F_LRA 200
// Library Selection 
#define LIB_SELECTION 6 // For Empty:0' TS2200 library A to D:1-5, LRA Library: 6 

#define DRV2605L_GREETING long_buzz_for_programmatic_stopping
#define DRV2605L_DEFAULT_MODE sharp_click


seq#	Sequence name	seq#	Sequence name	seq#	Sequence name
1	strong_click	43	lg_dblclick_med_60	85	transition_rampup_med_smooth2
2	strong_click_60	44	lg_dblsharp_tick	86	transition_rampup_short_smooth1
3	strong_click_30	45	lg_dblsharp_tick_80	87	transition_rampup_short_smooth2
4	sharp_click	46	lg_dblsharp_tick_60	88	transition_rampup_long_sharp1
5	sharp_click_60	47	buzz	89	transition_rampup_long_sharp2
6	sharp_click_30	48	buzz_80	90	transition_rampup_med_sharp1
7	soft_bump	49	buzz_60	91	transition_rampup_med_sharp2
8	soft_bump_60	50	buzz_40	92	transition_rampup_short_sharp1
9	soft_bump_30	51	buzz_20	93	transition_rampup_short_sharp2
10	dbl_click	52	pulsing_strong	94	transition_rampdown_long_smooth1_50
11	dbl_click_60	53	pulsing_strong_80	95	transition_rampdown_long_smooth2_50
12	trp_click	54	pulsing_medium	96	transition_rampdown_med_smooth1_50
13	soft_fuzz	55	pulsing_medium_80	97	transition_rampdown_med_smooth2_50
14	strong_buzz	56	pulsing_sharp	98	transition_rampdown_short_smooth1_50
15	alert_750ms	57	pulsing_sharp_80	99	transition_rampdown_short_smooth2_50
16	alert_1000ms	58	transition_click	100	transition_rampdown_long_sharp1_50
17	strong_click1	59	transition_click_80	101	transition_rampdown_long_sharp2_50
18	strong_click2_80	60	transition_click_60	102	transition_rampdown_med_sharp1_50
19	strong_click3_60	61	transition_click_40	103	transition_rampdown_med_sharp2_50
20	strong_click4_30	62	transition_click_20	104	transition_rampdown_short_sharp1_50
21	medium_click1	63	transition_click_10	105	transition_rampdown_short_sharp2_50
22	medium_click2_80	64	transition_hum	106	transition_rampup_long_smooth1_50
23	medium_click3_60	65	transition_hum_80	107	transition_rampup_long_smooth2_50
24	sharp_tick1	66	transition_hum_60	108	transition_rampup_med_smooth1_50
25	sharp_tick2_80	67	transition_hum_40	109	transition_rampup_med_smooth2_50
26	sharp_tick3_60	68	transition_hum_20	110	transition_rampup_short_smooth1_50
27	sh_dblclick_str	69	transition_hum_10	111	transition_rampup_short_smooth2_50
28	sh_dblclick_str_80	70	transition_rampdown_long_smooth1	112	transition_rampup_long_sharp1_50
29	sh_dblclick_str_60	71	transition_rampdown_long_smooth2	113	transition_rampup_long_sharp2_50
30	sh_dblclick_str_30	72	transition_rampdown_med_smooth1	114	transition_rampup_med_sharp1_50
31	sh_dblclick_med	73	transition_rampdown_med_smooth2	115	transition_rampup_med_sharp2_50
32	sh_dblclick_med_80	74	transition_rampdown_short_smooth1	116	transition_rampup_short_sharp1_50
33	sh_dblclick_med_60	75	transition_rampdown_short_smooth2	117	transition_rampup_short_sharp2_50
34	sh_dblsharp_tick	76	transition_rampdown_long_sharp1	118	long_buzz_for_programmatic_stopping
35	sh_dblsharp_tick_80	77	transition_rampdown_long_sharp2	119	smooth_hum1_50
36	sh_dblsharp_tick_60	78	transition_rampdown_med_sharp1	120	smooth_hum2_40
37	lg_dblclick_str	79	transition_rampdown_med_sharp2	121	smooth_hum3_30
38	lg_dblclick_str_80	80	transition_rampdown_short_sharp1	122	smooth_hum4_20
39	lg_dblclick_str_60	81	transition_rampdown_short_sharp2	123	smooth_hum5_10
40	lg_dblclick_str_30	82	transition_rampup_long_smooth1		
41	lg_dblclick_med	83	transition_rampup_long_smooth2		
42	lg_dblclick_med_80	84	transition_rampup_med_smooth1	
*/	
