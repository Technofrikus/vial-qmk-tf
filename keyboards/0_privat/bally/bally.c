/* Copyright 2020 Alexander Tulloh
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "quantum.h"
#include <math.h>
#include "bally.h"
#include "wait.h"
#include "debug.h"
#include <stdio.h>
#include "pointing_device.h"

#ifndef KC__VOLUP
#    define KC__VOLUP KC_KB_VOLUME_UP
#endif
#ifndef KC__VOLDOWN
#    define KC__VOLDOWN KC_KB_VOLUME_DOWN
#endif

extern const pointing_device_driver_t pointing_device_driver;


// Invert vertical scroll direction
#ifndef TRACKBALL_SCROLL_INV_DEFAULT
#    define TRACKBALL_SCROLL_INV_DEFAULT true
#endif

#ifndef TRACKBALL_CPI_OPTIONS
#    define TRACKBALL_CPI_OPTIONS { 500, 800, 1100, 1400, 1700, 2000 }
#    ifndef TRACKBALL_CPI_DEFAULT
#        define TRACKBALL_CPI_DEFAULT 2
#    endif
#endif
#ifndef TRACKBALL_CPI_DEFAULT
#    define TRACKBALL_CPI_DEFAULT 2
#endif

#ifndef TRACKBALL_SCROLL_DIVIDERS
#    define TRACKBALL_SCROLL_DIVIDERS { 16, 24, 32, 48, 64 }
#    ifndef TRACKBALL_SCROLL_DIV_DEFAULT
#        define TRACKBALL_SCROLL_DIV_DEFAULT 1
#    endif
#endif
#ifndef TRACKBALL_SCROLL_DIV_DEFAULT
#    define TRACKBALL_SCROLL_DIV_DEFAULT 1
#endif


#ifndef TRACKBALL_ROTATION_ANGLE
#    define TRACKBALL_ROTATION_ANGLE { -90, -75, -60, -45, -30, -15, 0, 15, 30, 45, 60, 75, 90 }
#    ifndef TRACKBALL_ROTATION_DEFAULT
#       define TRACKBALL_ROTATION_DEFAULT 3
#    endif
#endif
#ifndef TRACKBALL_ROTATION_DEFAULT
#    define TRACKBALL_ROTATION_DEFAULT 3
#endif
#ifndef TRACKBALL_AUTO_MOUSE_MODE
#    define TRACKBALL_AUTO_MOUSE_MODE false
#endif

/* Trackball Movement Configuration - Easy to test and revert */
// Enable/disable dynamic multiplier (causes uneven movement when enabled)
#ifndef TRACKBALL_ENABLE_DYNAMIC_MULTIPLIER
#    define TRACKBALL_ENABLE_DYNAMIC_MULTIPLIER false  // Set to true to re-enable
#endif

// Smoothing factor (0.0 = no smoothing, 1.0 = maximum smoothing)
// Lower values = more responsive but less smooth
// Higher values = smoother but more lag
#ifndef TRACKBALL_SMOOTHING_FACTOR
#    define TRACKBALL_SMOOTHING_FACTOR 0.7  // Original: 0.7, try 0.5 for less lag
#endif

// Base sensitivity multiplier (applied to all movements)
#ifndef TRACKBALL_SENSITIVITY_MULTIPLIER
#    define TRACKBALL_SENSITIVITY_MULTIPLIER 1.5  // Original: 1.5
#endif

// Movement sensitivity for accumulator (lower = smoother but less responsive)
#ifndef TRACKBALL_MOVEMENT_SENSITIVITY
#    define TRACKBALL_MOVEMENT_SENSITIVITY 0.5  // Original: 0.5, try 1.0 for more responsive
#endif

// Dynamic multiplier configuration (only used if TRACKBALL_ENABLE_DYNAMIC_MULTIPLIER is true)
#ifndef TRACKBALL_DYNAMIC_MULTIPLIER_DIVISOR
#    define TRACKBALL_DYNAMIC_MULTIPLIER_DIVISOR 10.0  // Original: 10.0, lower = more aggressive
#endif
#ifndef TRACKBALL_DYNAMIC_MULTIPLIER_MIN
#    define TRACKBALL_DYNAMIC_MULTIPLIER_MIN 0.5  // Original: 0.5
#endif
#ifndef TRACKBALL_DYNAMIC_MULTIPLIER_MAX
#    define TRACKBALL_DYNAMIC_MULTIPLIER_MAX 3.0  // Original: 3.0
#endif

#ifndef TRACKBALL_SCROLL_SENSITIVITY
#    define TRACKBALL_SCROLL_SENSITIVITY 0.35f
#endif

#ifndef TRACKBALL_VOLUME_SENSITIVITY
#    define TRACKBALL_VOLUME_SENSITIVITY 0.005f
#endif

#ifndef TRACKBALL_BRIGHTNESS_SENSITIVITY
#    define TRACKBALL_BRIGHTNESS_SENSITIVITY 0.02f
#endif

#ifndef TRACKBALL_CURSOR_SMOOTHING
#    define TRACKBALL_CURSOR_SMOOTHING TRACKBALL_SMOOTHING_FACTOR
#endif
#ifndef TRACKBALL_MODE_SMOOTHING
#    define TRACKBALL_MODE_SMOOTHING TRACKBALL_SMOOTHING_FACTOR
#endif
#ifndef TRACKBALL_CURSOR_DEADBAND
#    define TRACKBALL_CURSOR_DEADBAND 0.02f
#endif
#ifndef TRACKBALL_MODE_DEADBAND
#    define TRACKBALL_MODE_DEADBAND 0.01f
#endif

trackball_config_t trackball_config;
uint16_t cpi_array[] = TRACKBALL_CPI_OPTIONS;
uint16_t scrl_div_array[] = TRACKBALL_SCROLL_DIVIDERS;
uint16_t angle_array[] = TRACKBALL_ROTATION_ANGLE;
#define CPI_OPTION_SIZE (sizeof(cpi_array) / sizeof(uint16_t))
#define SCRL_DIV_SIZE (sizeof(scrl_div_array) / sizeof(uint16_t))
#define ANGLE_SIZE (sizeof(angle_array) / sizeof(uint16_t))

static bool scrl_mo_active = false;
static bool vol_mo_active = false;
static bool vol_toggle_active = false;
static bool brt_mo_active = false;
static bool brt_toggle_active = false;

typedef enum {
    TRACKBALL_MODE_CURSOR = 0,
    TRACKBALL_MODE_SCROLL,
    TRACKBALL_MODE_VOLUME,
    TRACKBALL_MODE_BRIGHTNESS,
} trackball_mode_t;

typedef struct {
    float rotation_cos;
    float rotation_sin;
    float cursor_prev_x;
    float cursor_prev_y;
    float mode_prev_x;
    float mode_prev_y;
    float cursor_accum_x;
    float cursor_accum_y;
    float scroll_accum_x;
    float scroll_accum_y;
    float volume_accum;
    float brightness_accum;
    trackball_mode_t last_mode;
} trackball_runtime_state_t;

static trackball_runtime_state_t trackball_state = {
    .rotation_cos = 1.0f,
    .rotation_sin = 0.0f,
    .last_mode = TRACKBALL_MODE_CURSOR,
};

static inline float trackball_apply_smoothing(float *prev, float input, float factor) {
    float value = (*prev * factor) + (input * (1.0f - factor));
    *prev = value;
    return value;
}

static inline float trackball_apply_deadband(float value, float threshold) {
    return (fabsf(value) < threshold) ? 0.0f : value;
}

static void trackball_refresh_rotation_cache(void) {
    float deg = angle_array[trackball_config.rotation_angle];
    float rad = deg * ((float)M_PI / 180.0f) * -1.0f;
    trackball_state.rotation_cos = cosf(rad);
    trackball_state.rotation_sin = sinf(rad);
}

static trackball_mode_t trackball_resolve_mode(void) {
    if (trackball_get_volume_mode()) {
        return TRACKBALL_MODE_VOLUME;
    }
    if (trackball_get_brightness_mode()) {
        return TRACKBALL_MODE_BRIGHTNESS;
    }
    if (trackball_get_scroll_mode()) {
        return TRACKBALL_MODE_SCROLL;
    }
    return TRACKBALL_MODE_CURSOR;
}

static void trackball_handle_mode_transition(trackball_mode_t new_mode, float seed_x, float seed_y) {
    if (trackball_state.last_mode == new_mode) {
        return;
    }
    switch (new_mode) {
        case TRACKBALL_MODE_CURSOR:
            trackball_state.cursor_accum_x = 0.0f;
            trackball_state.cursor_accum_y = 0.0f;
            break;
        case TRACKBALL_MODE_SCROLL:
            trackball_state.scroll_accum_x = 0.0f;
            trackball_state.scroll_accum_y = 0.0f;
            break;
        case TRACKBALL_MODE_VOLUME:
            trackball_state.volume_accum = 0.0f;
            break;
        case TRACKBALL_MODE_BRIGHTNESS:
            trackball_state.brightness_accum = 0.0f;
            break;
    }
    trackball_state.mode_prev_x = seed_x;
    trackball_state.mode_prev_y = seed_y;
    trackball_state.last_mode = new_mode;
}

#ifdef CONSOLE_ENABLE
static void log_scroll_status(const char *tag) {
    uint8_t div_idx = trackball_config.scrl_div;
    uint16_t divisor = scrl_div_array[div_idx];
    int32_t base_milli = (int32_t)(TRACKBALL_SCROLL_SENSITIVITY * 1000.0f + 0.5f);
    int32_t effective_milli = divisor ? base_milli / (int32_t)divisor : base_milli;
    xprintf("SCRL[%s] idx=%u div=%u base=%ld/1000 eff=%ld/1000 toggle=%u hold=%u\n",
            tag,
            div_idx,
            divisor,
            (long)base_milli,
            (long)effective_milli,
            trackball_config.scrl_mode,
            scrl_mo_active);
}

static void log_volume_status(const char *tag) {
    int32_t base_milli = (int32_t)(TRACKBALL_VOLUME_SENSITIVITY * 1000.0f + 0.5f);
    xprintf("VOL[%s] base=%ld/1000 toggle=%u hold=%u\n",
            tag,
            (long)base_milli,
            vol_toggle_active,
            vol_mo_active);
}

static void log_brightness_status(const char *tag) {
    int32_t base_milli = (int32_t)(TRACKBALL_BRIGHTNESS_SENSITIVITY * 1000.0f + 0.5f);
    xprintf("BRT[%s] base=%ld/1000 toggle=%u hold=%u\n",
            tag,
            (long)base_milli,
            brt_toggle_active,
            brt_mo_active);
}
#else
#    define log_scroll_status(tag) do { } while (0)
#    define log_volume_status(tag) do { } while (0)
#    define log_brightness_status(tag) do { } while (0)
#endif

__attribute__((weak)) void trackball_handle_volume_step(int8_t direction) {
    // Adjust these keycodes if your OS uses different volume shortcuts
    if (direction > 0) {
        tap_code16(LSA(KC__VOLUP));
    } else if (direction < 0) {
        tap_code16(LSA(KC__VOLDOWN));
    }
}

__attribute__((weak)) void trackball_handle_brightness_step(int8_t direction) {
    // Adjust these keycodes if your OS uses different brightness shortcuts
    if (direction > 0) {
        tap_code(KC_BRIGHTNESS_UP);
    } else if (direction < 0) {
        tap_code(KC_BRIGHTNESS_DOWN);
    }
}

void pointing_device_init_kb(void) {
    // Load config from EEPROM first
    trackball_config.raw = eeconfig_read_kb();
    
    // Validate and set CPI
    if (trackball_config.cpi_idx >= CPI_OPTION_SIZE) {
        trackball_config.cpi_idx = TRACKBALL_CPI_DEFAULT;
    }
    pointing_device_set_cpi(cpi_array[trackball_config.cpi_idx]);
    
    eeconfig_update_kb(trackball_config.raw);
    //set_auto_mouse_layer(4);
    set_auto_mouse_enable(trackball_config.auto_mouse);
    log_scroll_status("init");
}


report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    float sensitivity = TRACKBALL_MOVEMENT_SENSITIVITY;
    float sensitivity_multiplier = TRACKBALL_SENSITIVITY_MULTIPLIER;

    float rotated_x = -(mouse_report.x * trackball_state.rotation_cos - mouse_report.y * trackball_state.rotation_sin);
    float rotated_y = mouse_report.x * trackball_state.rotation_sin + mouse_report.y * trackball_state.rotation_cos;

    float cursor_smoothed_x = trackball_apply_smoothing(&trackball_state.cursor_prev_x, rotated_x, TRACKBALL_CURSOR_SMOOTHING);
    float cursor_smoothed_y = trackball_apply_smoothing(&trackball_state.cursor_prev_y, rotated_y, TRACKBALL_CURSOR_SMOOTHING);

    trackball_mode_t active_mode = trackball_resolve_mode();
    trackball_handle_mode_transition(active_mode, rotated_x, rotated_y);

    float mode_smoothed_x = trackball_apply_smoothing(&trackball_state.mode_prev_x, rotated_x, TRACKBALL_MODE_SMOOTHING);
    float mode_smoothed_y = trackball_apply_smoothing(&trackball_state.mode_prev_y, rotated_y, TRACKBALL_MODE_SMOOTHING);

#if TRACKBALL_ENABLE_DYNAMIC_MULTIPLIER
    float movement_magnitude = sqrtf(cursor_smoothed_x * cursor_smoothed_x + cursor_smoothed_y * cursor_smoothed_y);
    float dynamic_multiplier = 1.0f + movement_magnitude / TRACKBALL_DYNAMIC_MULTIPLIER_DIVISOR;
    dynamic_multiplier = fminf(fmaxf(dynamic_multiplier, TRACKBALL_DYNAMIC_MULTIPLIER_MIN), TRACKBALL_DYNAMIC_MULTIPLIER_MAX);
    cursor_smoothed_x *= sensitivity_multiplier * dynamic_multiplier;
    cursor_smoothed_y *= sensitivity_multiplier * dynamic_multiplier;
#else
    cursor_smoothed_x *= sensitivity_multiplier;
    cursor_smoothed_y *= sensitivity_multiplier;
#endif

    mouse_report.h = 0;
    mouse_report.v = 0;

    if (active_mode == TRACKBALL_MODE_VOLUME) {
        float mode_filtered_y = trackball_apply_deadband(mode_smoothed_y, TRACKBALL_MODE_DEADBAND);
        trackball_state.volume_accum -= mode_filtered_y * TRACKBALL_VOLUME_SENSITIVITY;

        while (trackball_state.volume_accum >= 1.0f) {
            trackball_handle_volume_step(1);
            trackball_state.volume_accum -= 1.0f;
        }
        while (trackball_state.volume_accum <= -1.0f) {
            trackball_handle_volume_step(-1);
            trackball_state.volume_accum += 1.0f;
        }

        mouse_report.x = 0;
        mouse_report.y = 0;
    } else if (active_mode == TRACKBALL_MODE_BRIGHTNESS) {
        float mode_filtered_y = trackball_apply_deadband(mode_smoothed_y, TRACKBALL_MODE_DEADBAND);
        trackball_state.brightness_accum -= mode_filtered_y * TRACKBALL_BRIGHTNESS_SENSITIVITY;

        while (trackball_state.brightness_accum >= 1.0f) {
            trackball_handle_brightness_step(1);
            trackball_state.brightness_accum -= 1.0f;
        }
        while (trackball_state.brightness_accum <= -1.0f) {
            trackball_handle_brightness_step(-1);
            trackball_state.brightness_accum += 1.0f;
        }

        mouse_report.x = 0;
        mouse_report.y = 0;
    } else if (active_mode == TRACKBALL_MODE_SCROLL) {
        float scroll_x = mode_smoothed_x;
        float scroll_y = mode_smoothed_y;

        if (fabsf(scroll_x) > fabsf(scroll_y)) {
            scroll_y = 0.0f;
        } else {
            scroll_x = 0.0f;
        }

        scroll_x = trackball_apply_deadband(scroll_x, TRACKBALL_MODE_DEADBAND);
        scroll_y = trackball_apply_deadband(scroll_y, TRACKBALL_MODE_DEADBAND);

        float horiz_factor = trackball_config.scrl_inv ? 1.0f : -1.0f;
        float vert_factor = trackball_config.scrl_inv ? -1.0f : 1.0f;
        trackball_state.scroll_accum_x += horiz_factor * scroll_x * TRACKBALL_SCROLL_SENSITIVITY;
        trackball_state.scroll_accum_y += vert_factor * scroll_y * TRACKBALL_SCROLL_SENSITIVITY;

        float scroll_divisor = (float)scrl_div_array[trackball_config.scrl_div];
        int8_t h_scroll = 0;
        if (fabsf(trackball_state.scroll_accum_x) >= scroll_divisor) {
            float steps = trackball_state.scroll_accum_x / scroll_divisor;
            steps = fmaxf(-127.0f, fminf(127.0f, steps));
            h_scroll = (int8_t)steps;
            trackball_state.scroll_accum_x -= (float)h_scroll * scroll_divisor;
        }

        int8_t v_scroll = 0;
        if (fabsf(trackball_state.scroll_accum_y) >= scroll_divisor) {
            float steps = trackball_state.scroll_accum_y / scroll_divisor;
            steps = fmaxf(-127.0f, fminf(127.0f, steps));
            v_scroll = (int8_t)steps;
            trackball_state.scroll_accum_y -= (float)v_scroll * scroll_divisor;
        }

        if (h_scroll != 0) {
            mouse_report.h += h_scroll;
        }
        if (v_scroll != 0) {
            mouse_report.v += v_scroll;
        }

        mouse_report.x = 0;
        mouse_report.y = 0;
    } else {
        float cursor_filtered_x = trackball_apply_deadband(cursor_smoothed_x, TRACKBALL_CURSOR_DEADBAND);
        float cursor_filtered_y = trackball_apply_deadband(cursor_smoothed_y, TRACKBALL_CURSOR_DEADBAND);

        trackball_state.cursor_accum_x += cursor_filtered_x * sensitivity;
        trackball_state.cursor_accum_y += cursor_filtered_y * sensitivity;

        if (fabsf(trackball_state.cursor_accum_x) >= 1.0f) {
            int8_t delta = (int8_t)trackball_state.cursor_accum_x;
            mouse_report.x = delta;
            trackball_state.cursor_accum_x -= delta;
        } else {
            mouse_report.x = 0;
        }

        if (fabsf(trackball_state.cursor_accum_y) >= 1.0f) {
            int8_t delta = (int8_t)trackball_state.cursor_accum_y;
            mouse_report.y = delta;
            trackball_state.cursor_accum_y -= delta;
        } else {
            mouse_report.y = 0;
        }
    }

    return pointing_device_task_user(mouse_report);
}


bool process_record_kb(uint16_t keycode, keyrecord_t* record) {
    // xprintf("KL: kc: %u, col: %u, row: %u, pressed: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed);
    
    // Debug: Check if CPI_SW keycode is received
    #ifdef CONSOLE_ENABLE
    if (keycode == CPI_SW) {
        xprintf("CPI_SW detected: keycode=%u, CPI_SW=%u, pressed=%u\n", keycode, CPI_SW, record->event.pressed);
    }
    #endif
    
    // Process CPI_SW BEFORE process_record_user to ensure it's handled
    if (keycode == CPI_SW && record->event.pressed) {
        uint8_t old_idx = trackball_config.cpi_idx;
        trackball_config.cpi_idx = (trackball_config.cpi_idx + 1) % CPI_OPTION_SIZE;
        
        // Debug output (enable CONSOLE_ENABLE in rules.mk to see this)
        #ifdef CONSOLE_ENABLE
        xprintf("CPI_SW: old_idx=%u, new_idx=%u, cpi=%u\n", old_idx, trackball_config.cpi_idx, cpi_array[trackball_config.cpi_idx]);
        #endif
        
        // Set CPI on sensor immediately
        pointing_device_set_cpi(cpi_array[trackball_config.cpi_idx]);
        
        // Save to EEPROM
        eeconfig_update_kb(trackball_config.raw);
        
        // Verify the value was saved correctly
        #ifdef CONSOLE_ENABLE
        trackball_config_t verify_config;
        verify_config.raw = eeconfig_read_kb();
        xprintf("CPI_SW verify: saved_idx=%u, current_idx=%u\n", verify_config.cpi_idx, trackball_config.cpi_idx);
        #endif
        
        return false; // Prevent further processing
    }
    
    if (!process_record_user(keycode, record)) return false;

    switch (keycode) {
        #ifndef MOUSEKEY_ENABLE
                // process KC_MS_BTN1~8 by myself
                // See process_action() in quantum/action.c for details.
                case KC_MS_BTN1 ... KC_MS_BTN8: {
                    extern void register_button(bool, enum mouse_buttons);
                    register_button(record->event.pressed, MOUSE_BTN_MASK(keycode - KC_MS_BTN1));
                    return false;
                }
        #endif
        //*
        case AM_TOG:
            if(record->event.pressed) { // key down
                //auto_mouse_layer_off(); // disable target layer if needed
                trackball_config.auto_mouse ^= 1;
                eeconfig_update_kb(trackball_config.raw);
                set_auto_mouse_enable(trackball_config.auto_mouse);
                //auto_mouse_tg_off = !get_auto_mouse_enable();
            } // do nothing on key up
            return false; // prevent further processing of keycode            
    //*/
    }

    if (keycode == SCRL_SW && record->event.pressed) {
        trackball_config.scrl_div = (trackball_config.scrl_div + 1) % SCRL_DIV_SIZE;
        eeconfig_update_kb(trackball_config.raw);
        log_scroll_status("SCRL_SW");
        return false;
    }

    if (keycode == ROT_R15 && record->event.pressed) {
        trackball_config.rotation_angle = (trackball_config.rotation_angle + 1) % ANGLE_SIZE;
        eeconfig_update_kb(trackball_config.raw);
        trackball_refresh_rotation_cache();
        return false;
    }

    if (keycode == ROT_L15 && record->event.pressed) {
        trackball_config.rotation_angle = (ANGLE_SIZE + trackball_config.rotation_angle - 1) % ANGLE_SIZE;
        eeconfig_update_kb(trackball_config.raw);
        trackball_refresh_rotation_cache();
        return false;
    }

    if (keycode == SCRL_IN && record->event.pressed) {
        trackball_config.scrl_inv ^= 1;
        eeconfig_update_kb(trackball_config.raw);
        log_scroll_status("SCRL_IN");
        return false;
    }

    if (keycode == SCRL_TO && record->event.pressed) {
        trackball_set_scroll_mode(!trackball_config.scrl_mode);
        eeconfig_update_kb(trackball_config.raw);
        log_scroll_status("SCRL_TO");
        return false;
    }

    if (keycode == SCRL_MO) {
        scrl_mo_active = record->event.pressed;
        log_scroll_status(scrl_mo_active ? "SCRL_MO(on)" : "SCRL_MO(off)");
        return false;
    }

    if (keycode == VOL_TO && record->event.pressed) {
        trackball_set_volume_mode(!vol_toggle_active);
        log_volume_status("VOL_TO");
        return false;
    }

    if (keycode == VOL_MO) {
        vol_mo_active = record->event.pressed;
        log_volume_status(vol_mo_active ? "VOL_MO(on)" : "VOL_MO(off)");
        return false;
    }

    if (keycode == BRT_TO && record->event.pressed) {
        trackball_set_brightness_mode(!brt_toggle_active);
        log_brightness_status("BRT_TO");
        return false;
    }

    if (keycode == BRT_MO) {
        brt_mo_active = record->event.pressed;
        log_brightness_status(brt_mo_active ? "BRT_MO(on)" : "BRT_MO(off)");
        return false;
    }

    return true;
}


layer_state_t layer_state_set_kb(layer_state_t state) {
    switch(get_highest_layer(remove_auto_mouse_layer(state, true))) {
        case 1 ... 2:
            //rgblight_sethsv_range(HSV_YELLOW, 0, 9);
            trackball_set_scroll_mode(true);
            state = remove_auto_mouse_layer(state, false);
            set_auto_mouse_enable(false);
            break;
        case 3 ... 7:
            //rgblight_sethsv_range(HSV_CYAN, 0, 9);
            trackball_set_scroll_mode(false);
            //set_auto_mouse_enable(true);
            break;
        default:
            //rgblight_sethsv_range(HSV_RED, 0, 9);
            trackball_set_scroll_mode(false);
            
            if (trackball_config.auto_mouse) {
                set_auto_mouse_enable(true);
            } else {
                //state = remove_auto_mouse_layer(state, false);
                set_auto_mouse_enable(false);
            }
            
            //set_auto_mouse_enable(true);
            //state = remove_auto_mouse_layer(state, false);
            //set_auto_mouse_enable(trackball_config.auto_mouse);
            break;
        }
    //rgblight_set_effect_range( 9, 36);
  return state;
};



void eeconfig_init_kb(void) {
    trackball_config.cpi_idx = TRACKBALL_CPI_DEFAULT;
    trackball_config.scrl_div = TRACKBALL_SCROLL_DIV_DEFAULT;
    trackball_config.rotation_angle = TRACKBALL_ROTATION_DEFAULT;
    trackball_config.scrl_inv = TRACKBALL_SCROLL_INV_DEFAULT;
    trackball_config.scrl_mode = false;
    trackball_config.auto_mouse = TRACKBALL_AUTO_MOUSE_MODE;
    scrl_mo_active = false;
    vol_mo_active = false;
    vol_toggle_active = false;
    brt_mo_active = false;
    brt_toggle_active = false;
    trackball_refresh_rotation_cache();
    eeconfig_update_kb(trackball_config.raw);
    eeconfig_init_user();
}


void matrix_init_kb(void) {
    // is safe to just read CPI setting since matrix init
    // comes before pointing device init.
    trackball_config.raw = eeconfig_read_kb();
    if (trackball_config.cpi_idx >= CPI_OPTION_SIZE) // || trackball_config.scrl_div > SCRL_DIV_SIZE || trackball_config.rotation_angle > ANGLE_SIZE)
    {
        eeconfig_init_kb();
    }
    matrix_init_user();
}


bool trackball_get_scroll_mode(void) {
    return scrl_mo_active || trackball_config.scrl_mode;
}

void trackball_set_scroll_mode(bool mode) {
    trackball_config.scrl_mode = mode;
}

bool trackball_get_volume_mode(void) {
    return vol_mo_active || vol_toggle_active;
}

void trackball_set_volume_mode(bool mode) {
    vol_toggle_active = mode;
}

bool trackball_get_brightness_mode(void) {
    return brt_mo_active || brt_toggle_active;
}

void trackball_set_brightness_mode(bool mode) {
    brt_toggle_active = mode;
}

