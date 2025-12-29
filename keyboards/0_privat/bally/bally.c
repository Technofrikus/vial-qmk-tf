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
#include "eeprom_driver.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>
#include "pointing_device.h"
#include "timer.h"
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
#    include "pointing_device_auto_mouse.h"
#endif

#ifndef KC__VOLUP
#    define KC__VOLUP KC_KB_VOLUME_UP
#endif
#ifndef KC__VOLDOWN
#    define KC__VOLDOWN KC_KB_VOLUME_DOWN
#endif

extern const pointing_device_driver_t pointing_device_driver;


#ifndef TRACKBALL_CONFIG_VERSION
#    define TRACKBALL_CONFIG_VERSION 3
#endif

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
#    define TRACKBALL_AUTO_MOUSE_MODE true
#endif

/* Trackball Movement Configuration - Easy to test and revert */
// Enable/disable dynamic multiplier (legacy) or adaptive dynamic gain
#ifndef TRACKBALL_ENABLE_DYNAMIC_MULTIPLIER
#    define TRACKBALL_ENABLE_DYNAMIC_MULTIPLIER false  // Set to true to enable legacy multiplier
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

#ifndef TRACKBALL_ENABLE_ADAPTIVE_GAIN
#    define TRACKBALL_ENABLE_ADAPTIVE_GAIN true
#endif
#ifndef TRACKBALL_ADAPTIVE_HISTORY_LENGTH
#    define TRACKBALL_ADAPTIVE_HISTORY_LENGTH 6
#endif
#ifndef TRACKBALL_ADAPTIVE_WINDOW_MS
#    define TRACKBALL_ADAPTIVE_WINDOW_MS 40
#endif
#ifndef TRACKBALL_ADAPTIVE_MIN_SPEED
#    define TRACKBALL_ADAPTIVE_MIN_SPEED 0.1f
#endif
#ifndef TRACKBALL_ADAPTIVE_SPEED_REF
#    define TRACKBALL_ADAPTIVE_SPEED_REF 5.0f
#endif
#ifndef TRACKBALL_ADAPTIVE_ACCEL_REF
#    define TRACKBALL_ADAPTIVE_ACCEL_REF 0.2f
#endif
#ifndef TRACKBALL_ADAPTIVE_SPEED_WEIGHT
#    define TRACKBALL_ADAPTIVE_SPEED_WEIGHT 0.6f
#endif
#ifndef TRACKBALL_ADAPTIVE_ACCEL_WEIGHT
#    define TRACKBALL_ADAPTIVE_ACCEL_WEIGHT 0.3f
#endif
#ifndef TRACKBALL_ADAPTIVE_DIRECTION_WEIGHT
#    define TRACKBALL_ADAPTIVE_DIRECTION_WEIGHT 0.1f
#endif
#ifndef TRACKBALL_ADAPTIVE_GAIN_BASE
#    define TRACKBALL_ADAPTIVE_GAIN_BASE 1.0f
#endif
#ifndef TRACKBALL_ADAPTIVE_GAIN_LIMIT
#    define TRACKBALL_ADAPTIVE_GAIN_LIMIT 3.5f
#endif
#ifndef TRACKBALL_ADAPTIVE_GAIN_SETTING_MIN
#    define TRACKBALL_ADAPTIVE_GAIN_SETTING_MIN 0
#endif
#ifndef TRACKBALL_ADAPTIVE_GAIN_SETTING_MAX
#    define TRACKBALL_ADAPTIVE_GAIN_SETTING_MAX 255
#endif
#ifndef TRACKBALL_ADAPTIVE_GAIN_SETTING_DEFAULT
#    define TRACKBALL_ADAPTIVE_GAIN_SETTING_DEFAULT 192
#endif
#ifndef TRACKBALL_ADAPTIVE_GAIN_SETTING_STEP
#    define TRACKBALL_ADAPTIVE_GAIN_SETTING_STEP 16
#endif

#ifndef TRACKBALL_TYPING_GUARD_TIMEOUT_MS
#    define TRACKBALL_TYPING_GUARD_TIMEOUT_MS 175
#endif
#ifndef TRACKBALL_TYPING_GUARD_THRESHOLD
#    define TRACKBALL_TYPING_GUARD_THRESHOLD 0.99f
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

// Scroll debugging - set to 1 to enable detailed scroll logging via HID console
// Requires CONSOLE_ENABLE to be enabled in rules.mk
#ifndef TRACKBALL_SCROLL_DEBUG
#    define TRACKBALL_SCROLL_DEBUG 1  // Set to 1 to enable scroll debugging
#endif

trackball_config_t trackball_config;
uint16_t cpi_array[] = TRACKBALL_CPI_OPTIONS;
uint16_t scrl_div_array[] = TRACKBALL_SCROLL_DIVIDERS;
uint16_t angle_array[] = TRACKBALL_ROTATION_ANGLE;
#define CPI_OPTION_SIZE (sizeof(cpi_array) / sizeof(uint16_t))
#define SCRL_DIV_SIZE (sizeof(scrl_div_array) / sizeof(uint16_t))
#define ANGLE_SIZE (sizeof(angle_array) / sizeof(uint16_t))

// --- Config packing helpers (32-bit eeconfig_kb storage) ---
static uint32_t trackball_config_pack(const trackball_config_t* cfg) {
    uint32_t raw = 0;
    raw |= (cfg->cpi_idx & 0x7);                 // bits 0-2
    raw |= ((cfg->scrl_div & 0x7) << 3);         // bits 3-5
    raw |= ((cfg->rotation_angle & 0xF) << 6);   // bits 6-9
    raw |= ((cfg->auto_mouse & 0x1) << 10);      // bit 10
    raw |= ((cfg->scrl_inv & 0x1) << 11);        // bit 11
    raw |= ((cfg->scrl_mode & 0x1) << 12);       // bit 12
    raw |= ((uint32_t)cfg->adaptive_gain << 13); // bits 13-20
    raw |= ((cfg->reserved & 0x7) << 21);        // bits 21-23
    return raw;
}

static void trackball_config_unpack(uint32_t raw, trackball_config_t* cfg) {
    cfg->cpi_idx        = raw & 0x7;
    cfg->scrl_div       = (raw >> 3) & 0x7;
    cfg->rotation_angle = (raw >> 6) & 0xF;
    cfg->auto_mouse     = (raw >> 10) & 0x1;
    cfg->scrl_inv       = (raw >> 11) & 0x1;
    cfg->scrl_mode      = (raw >> 12) & 0x1;
    cfg->adaptive_gain  = (raw >> 13) & 0xFF;
    cfg->reserved       = (raw >> 21) & 0x7;
}
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
    float vx;
    float vy;
    uint32_t timestamp;
} trackball_motion_sample_t;

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
    trackball_motion_sample_t history[TRACKBALL_ADAPTIVE_HISTORY_LENGTH];
    uint8_t history_count;
    uint32_t typing_guard_timer;
    bool typing_guard_active;
} trackball_runtime_state_t;

static trackball_runtime_state_t trackball_state = {
    .rotation_cos = 1.0f,
    .rotation_sin = 0.0f,
    .last_mode = TRACKBALL_MODE_CURSOR,
    .history_count = 0,
    .typing_guard_timer = 0,
    .typing_guard_active = false,
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

static void trackball_history_reset(void) {
    trackball_state.history_count = 0;
}

static void trackball_history_push(float vx, float vy, uint32_t timestamp) {
#if TRACKBALL_ENABLE_ADAPTIVE_GAIN
    if (trackball_state.history_count == TRACKBALL_ADAPTIVE_HISTORY_LENGTH) {
        memmove(&trackball_state.history[0], &trackball_state.history[1],
                sizeof(trackball_motion_sample_t) * (TRACKBALL_ADAPTIVE_HISTORY_LENGTH - 1));
        trackball_state.history_count = TRACKBALL_ADAPTIVE_HISTORY_LENGTH - 1;
    }

    trackball_state.history[trackball_state.history_count++] = (trackball_motion_sample_t){
        .vx = vx,
        .vy = vy,
        .timestamp = timestamp,
    };

    uint32_t newest = trackball_state.history[trackball_state.history_count - 1].timestamp;
    uint8_t keep = 0;
    for (uint8_t i = 0; i < trackball_state.history_count; ++i) {
        uint32_t age = newest - trackball_state.history[i].timestamp;
        if (age <= TRACKBALL_ADAPTIVE_WINDOW_MS) {
            if (keep != i) {
                trackball_state.history[keep] = trackball_state.history[i];
            }
            keep++;
        }
    }
    trackball_state.history_count = keep;
#else
    (void)vx;
    (void)vy;
    (void)timestamp;
#endif
}

static float trackball_compute_adaptive_gain(void) {
#if !TRACKBALL_ENABLE_ADAPTIVE_GAIN
    return 1.0f;
#else
    if (trackball_config.adaptive_gain <= TRACKBALL_ADAPTIVE_GAIN_SETTING_MIN) {
        return 1.0f;
    }
    if (trackball_state.history_count < 2) {
        return 1.0f;
    }

    float sum_speed = 0.0f;
    float sum_accel = 0.0f;
    float sum_dir = 0.0f;
    uint8_t pair_count = 0;

    for (uint8_t i = 1; i < trackball_state.history_count; ++i) {
        trackball_motion_sample_t prev = trackball_state.history[i - 1];
        trackball_motion_sample_t curr = trackball_state.history[i];

        float prev_speed = sqrtf(prev.vx * prev.vx + prev.vy * prev.vy);
        float curr_speed = sqrtf(curr.vx * curr.vx + curr.vy * curr.vy);
        uint32_t dt_ms = curr.timestamp - prev.timestamp;
        if (dt_ms == 0) {
            dt_ms = 1;
        }

        sum_speed += curr_speed;
        float accel = (curr_speed - prev_speed) / (float)dt_ms;
        sum_accel += accel;

        if (prev_speed > 0.0f && curr_speed > 0.0f) {
            float dot = (prev.vx * curr.vx + prev.vy * curr.vy) / (prev_speed * curr_speed);
            if (dot > 1.0f) {
                dot = 1.0f;
            } else if (dot < -1.0f) {
                dot = -1.0f;
            }
            sum_dir += dot;
        }

        pair_count++;
    }

    if (pair_count == 0) {
        return 1.0f;
    }

    float avg_speed = sum_speed / pair_count;
    if (avg_speed < TRACKBALL_ADAPTIVE_MIN_SPEED) {
        return 1.0f;
    }

    float avg_accel = sum_accel / pair_count;
    float avg_dir = sum_dir / pair_count;

    float speed_term = fminf(avg_speed / TRACKBALL_ADAPTIVE_SPEED_REF, 1.0f);
    float accel_term = fminf(fmaxf(avg_accel, 0.0f) / TRACKBALL_ADAPTIVE_ACCEL_REF, 1.0f);
    float direction_term = (avg_dir + 1.0f) * 0.5f;

    float weighted = (speed_term * TRACKBALL_ADAPTIVE_SPEED_WEIGHT) +
                     (accel_term * TRACKBALL_ADAPTIVE_ACCEL_WEIGHT) +
                     (direction_term * TRACKBALL_ADAPTIVE_DIRECTION_WEIGHT);
    float weight_sum = TRACKBALL_ADAPTIVE_SPEED_WEIGHT + TRACKBALL_ADAPTIVE_ACCEL_WEIGHT + TRACKBALL_ADAPTIVE_DIRECTION_WEIGHT;
    float normalized = (weight_sum > 0.0f) ? (weighted / weight_sum) : speed_term;

    float denom = (float)(TRACKBALL_ADAPTIVE_GAIN_SETTING_MAX - TRACKBALL_ADAPTIVE_GAIN_SETTING_MIN);
    if (denom <= 0.0f) {
        return 1.0f;
    }

    float user_intensity = (float)(trackball_config.adaptive_gain - TRACKBALL_ADAPTIVE_GAIN_SETTING_MIN) / denom;
    float dynamic_range = TRACKBALL_ADAPTIVE_GAIN_LIMIT - TRACKBALL_ADAPTIVE_GAIN_BASE;
    float gain = TRACKBALL_ADAPTIVE_GAIN_BASE + (normalized * user_intensity * dynamic_range);

    if (gain < TRACKBALL_ADAPTIVE_GAIN_BASE) {
        gain = TRACKBALL_ADAPTIVE_GAIN_BASE;
    } else if (gain > TRACKBALL_ADAPTIVE_GAIN_LIMIT) {
        gain = TRACKBALL_ADAPTIVE_GAIN_LIMIT;
    }
    return gain;
#endif
}

static bool trackball_is_typing_key(uint16_t keycode) {
    if (IS_MOUSEKEY(keycode)) {
        return false;
    }
    if (keycode >= QK_KB && keycode <= QK_KB_MAX) {
        return false;
    }
    return true;
}

static void trackball_note_typing_event(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return;
    }
#ifdef POINTING_DEVICE_AUTO_MOUSE_ENABLE
    if (layer_state_is(AUTO_MOUSE_TARGET_LAYER)) {
        return;
    }
#endif
    if (!trackball_is_typing_key(keycode)) {
        return;
    }
    trackball_state.typing_guard_active = true;
    trackball_state.typing_guard_timer  = timer_read32();
}

static bool trackball_typing_guard_should_suppress(float x, float y) {
    if (!trackball_state.typing_guard_active) {
        return false;
    }
    if (timer_elapsed32(trackball_state.typing_guard_timer) > TRACKBALL_TYPING_GUARD_TIMEOUT_MS) {
        trackball_state.typing_guard_active = false;
        return false;
    }
    if (fabsf(x) <= TRACKBALL_TYPING_GUARD_THRESHOLD && fabsf(y) <= TRACKBALL_TYPING_GUARD_THRESHOLD) {
        return true;
    }
    trackball_state.typing_guard_active = false;
    return false;
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

static const char *trackball_mode_name(trackball_mode_t mode) {
    switch (mode) {
        case TRACKBALL_MODE_CURSOR:
            return "cursor";
        case TRACKBALL_MODE_SCROLL:
            return "scroll";
        case TRACKBALL_MODE_VOLUME:
            return "volume";
        case TRACKBALL_MODE_BRIGHTNESS:
            return "brightness";
        default:
            return "unknown";
    }
}

#if TRACKBALL_SCROLL_DEBUG && defined(CONSOLE_ENABLE)
// Scroll debug functions - defined here before first use
static void log_scroll_debug_mode_transition(trackball_mode_t old_mode, trackball_mode_t new_mode, bool scrl_mo, bool scrl_toggle) {
    xprintf("SCRL_DEBUG[MODE] %s -> %s (mo=%u toggle=%u)\n",
            trackball_mode_name(old_mode),
            trackball_mode_name(new_mode),
            scrl_mo ? 1 : 0,
            scrl_toggle ? 1 : 0);
}

static void log_scroll_debug_movement(float raw_x, float raw_y, float smoothed_x, float smoothed_y, float filtered_x, float filtered_y) {
    // Only log movement every 2 seconds and if there's significant movement
    static uint32_t last_mov_log_time = 0;
    uint32_t now = timer_read32();
    if (timer_elapsed32(last_mov_log_time) > 2000 && (fabsf(filtered_x) > 0.01f || fabsf(filtered_y) > 0.01f)) {
        // Convert floats to integers for display (multiply by 1000 to show 3 decimal places)
        int32_t raw_x_i = (int32_t)(raw_x * 1000.0f);
        int32_t raw_y_i = (int32_t)(raw_y * 1000.0f);
        int32_t smooth_x_i = (int32_t)(smoothed_x * 1000.0f);
        int32_t smooth_y_i = (int32_t)(smoothed_y * 1000.0f);
        int32_t filt_x_i = (int32_t)(filtered_x * 1000.0f);
        int32_t filt_y_i = (int32_t)(filtered_y * 1000.0f);
        xprintf("SCRL_DEBUG[MOV] raw=(%ld,%ld) smooth=(%ld,%ld) filt=(%ld,%ld)\n",
                (long)raw_x_i, (long)raw_y_i, (long)smooth_x_i, (long)smooth_y_i, (long)filt_x_i, (long)filt_y_i);
        last_mov_log_time = now;
    }
}

static void log_scroll_debug_accumulator(float accum_x, float accum_y, float divisor) {
    // Only log accumulator every 2 seconds and when it's significant or near threshold
    static uint32_t last_accum_log_time = 0;
    uint32_t now = timer_read32();
    if (timer_elapsed32(last_accum_log_time) > 2000 && 
        (fabsf(accum_x) > 1.0f || fabsf(accum_y) > 1.0f || fabsf(accum_x) >= divisor * 0.8f || fabsf(accum_y) >= divisor * 0.8f)) {
        // Convert floats to integers for display
        int32_t accum_x_i = (int32_t)(accum_x * 1000.0f);
        int32_t accum_y_i = (int32_t)(accum_y * 1000.0f);
        int32_t divisor_i = (int32_t)divisor;
        xprintf("SCRL_DEBUG[ACCUM] x=%ld y=%ld div=%ld\n", (long)accum_x_i, (long)accum_y_i, (long)divisor_i);
        last_accum_log_time = now;
    }
}

static void log_scroll_debug_event(int8_t h_scroll, int8_t v_scroll, float accum_x_after, float accum_y_after) {
    if (h_scroll != 0 || v_scroll != 0) {
        // Convert floats to integers for display
        int32_t accum_x_i = (int32_t)(accum_x_after * 1000.0f);
        int32_t accum_y_i = (int32_t)(accum_y_after * 1000.0f);
        xprintf("SCRL_DEBUG[EVENT] h=%d v=%d accum_after=(%ld,%ld)\n",
                h_scroll, v_scroll, (long)accum_x_i, (long)accum_y_i);
    }
}

static void log_scroll_debug_no_event(const char *reason, float accum_x, float accum_y, float divisor) {
    // Only log no-event every 3 seconds and when accumulator is significant to avoid spam
    static uint32_t last_no_event_log_time = 0;
    uint32_t now = timer_read32();
    if (timer_elapsed32(last_no_event_log_time) > 3000 && 
        (fabsf(accum_x) > divisor * 0.5f || fabsf(accum_y) > divisor * 0.5f)) {
        // Convert floats to integers for display
        int32_t accum_x_i = (int32_t)(accum_x * 1000.0f);
        int32_t accum_y_i = (int32_t)(accum_y * 1000.0f);
        int32_t divisor_i = (int32_t)divisor;
        xprintf("SCRL_DEBUG[NO_EVENT] reason=%s accum=(%ld,%ld) div=%ld\n",
                reason, (long)accum_x_i, (long)accum_y_i, (long)divisor_i);
        last_no_event_log_time = now;
    }
}
#endif

static void trackball_handle_mode_transition(trackball_mode_t new_mode, float seed_x, float seed_y) {
    if (trackball_state.last_mode == new_mode) {
        return;
    }
#ifdef CONSOLE_ENABLE
#if TRACKBALL_SCROLL_DEBUG
    // Only log mode transitions when SCRL_MO is involved (entering or leaving scroll mode via SCRL_MO)
    // This prevents logging when scroll mode is activated via layers
    if ((new_mode == TRACKBALL_MODE_SCROLL && scrl_mo_active) || 
        (trackball_state.last_mode == TRACKBALL_MODE_SCROLL && !scrl_mo_active)) {
        log_scroll_debug_mode_transition(trackball_state.last_mode, new_mode, scrl_mo_active, trackball_config.scrl_mode);
    }
#endif
#endif
    trackball_history_reset();
    switch (new_mode) {
        case TRACKBALL_MODE_CURSOR:
            trackball_state.cursor_accum_x = 0.0f;
            trackball_state.cursor_accum_y = 0.0f;
            break;
        case TRACKBALL_MODE_SCROLL:
            trackball_state.scroll_accum_x = 0.0f;
            trackball_state.scroll_accum_y = 0.0f;
#ifdef CONSOLE_ENABLE
#if TRACKBALL_SCROLL_DEBUG
            xprintf("SCRL_DEBUG[INIT] Scroll mode activated, accumulators reset\n");
#endif
#endif
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

static void trackball_emit_gain_status(const char *tag) {
    trackball_mode_t mode = trackball_resolve_mode();
    uint16_t denom = TRACKBALL_ADAPTIVE_GAIN_SETTING_MAX - TRACKBALL_ADAPTIVE_GAIN_SETTING_MIN;
    uint16_t pct = (denom > 0)
                       ? (uint16_t)(((uint32_t)(trackball_config.adaptive_gain - TRACKBALL_ADAPTIVE_GAIN_SETTING_MIN) * 100U) / denom)
                       : 0;
    uprintf("GAIN[%s] val=%u/%u (%u%%) mode=%s\n",
            tag,
            trackball_config.adaptive_gain,
            TRACKBALL_ADAPTIVE_GAIN_SETTING_MAX,
            pct,
            trackball_mode_name(mode));
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
#    define trackball_emit_gain_status(tag) do { } while (0)
#endif

// Forward declarations for scroll debug functions (needed before trackball_handle_mode_transition)
#if TRACKBALL_SCROLL_DEBUG && defined(CONSOLE_ENABLE)
// Forward declarations - functions defined later in file
void log_scroll_debug_mode_transition(trackball_mode_t old_mode, trackball_mode_t new_mode, bool scrl_mo, bool scrl_toggle);
void log_scroll_debug_movement(float raw_x, float raw_y, float smoothed_x, float smoothed_y, float filtered_x, float filtered_y);
void log_scroll_debug_accumulator(float accum_x, float accum_y, float divisor);
void log_scroll_debug_event(int8_t h_scroll, int8_t v_scroll, float accum_x_after, float accum_y_after);
void log_scroll_debug_no_event(const char *reason, float accum_x, float accum_y, float divisor);
#endif

// trackball_nudge_gain() removed - logic moved directly into process_record_kb() like CPI_SW

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

#if !TRACKBALL_SCROLL_DEBUG || !defined(CONSOLE_ENABLE)
// Disable scroll debugging when TRACKBALL_SCROLL_DEBUG is 0 or CONSOLE_ENABLE is disabled
#    define log_scroll_debug_mode_transition(old_mode, new_mode, scrl_mo, scrl_toggle) do { } while (0)
#    define log_scroll_debug_movement(raw_x, raw_y, smoothed_x, smoothed_y, filtered_x, filtered_y) do { } while (0)
#    define log_scroll_debug_accumulator(accum_x, accum_y, divisor) do { } while (0)
#    define log_scroll_debug_event(h_scroll, v_scroll, accum_x_after, accum_y_after) do { } while (0)
#    define log_scroll_debug_no_event(reason, accum_x, accum_y, divisor) do { } while (0)
#endif

void pointing_device_init_kb(void) {
    trackball_config_unpack(eeconfig_read_kb(), &trackball_config);
    bool needs_save       = false;

#ifdef CONSOLE_ENABLE
    // Debug: Log initial gain value after reading from EEPROM
    xprintf("GAIN[INIT] Read from EEPROM: adaptive_gain=%u reserved=%u raw=0x%08lx\n",
            trackball_config.adaptive_gain, trackball_config.reserved,
            (unsigned long)trackball_config_pack(&trackball_config));
    xprintf("GAIN[INIT] Full structure: cpi=%u scrl_div=%u rot=%u auto_mouse=%u scrl_inv=%u scrl_mode=%u gain=%u reserved=%u\n",
            trackball_config.cpi_idx, trackball_config.scrl_div, trackball_config.rotation_angle,
            trackball_config.auto_mouse, trackball_config.scrl_inv, trackball_config.scrl_mode,
            trackball_config.adaptive_gain, trackball_config.reserved);
#endif

    if (trackball_config.reserved != TRACKBALL_CONFIG_VERSION) {
#ifdef CONSOLE_ENABLE
        xprintf("GAIN[INIT] Reserved mismatch: read=%u expected=%u\n", 
                trackball_config.reserved, TRACKBALL_CONFIG_VERSION);
#endif
        if (trackball_config.reserved == 0xFF || trackball_config.reserved == 0x00) {
#ifdef CONSOLE_ENABLE
            xprintf("GAIN[INIT] EEPROM uninitialized (0xFF/0x00), calling eeconfig_init_kb() - THIS WILL RESET ALL VALUES!\n");
#endif
            eeconfig_init_kb();
            // Read back after init - but note that eeconfig_init_kb() should have already saved
            trackball_config_unpack(eeconfig_read_kb(), &trackball_config);
#ifdef CONSOLE_ENABLE
            xprintf("GAIN[INIT] After init: adaptive_gain=%u reserved=%u raw=0x%08lx\n",
                    trackball_config.adaptive_gain, trackball_config.reserved,
                    (unsigned long)trackball_config_pack(&trackball_config));
            // If reserved is still 0, eeconfig_init_kb() didn't save properly
            if (trackball_config.reserved == 0) {
                xprintf("GAIN[INIT] ERROR: eeconfig_init_kb() did not save! reserved is still 0!\n");
            }
#endif
        } else {
            if (trackball_config.reserved < 3) {
                trackball_config.auto_mouse = true;
            }
            trackball_config.reserved = TRACKBALL_CONFIG_VERSION;
            needs_save                = true;
#ifdef CONSOLE_ENABLE
            xprintf("GAIN[INIT] Updating reserved from %u to %u, needs_save=true\n", 
                    trackball_config.reserved, TRACKBALL_CONFIG_VERSION);
#endif
        }
    }

    // Validate and fix CPI index
    if (trackball_config.cpi_idx >= CPI_OPTION_SIZE) {
        trackball_config.cpi_idx = TRACKBALL_CPI_DEFAULT;
        needs_save = true;
    }
    // Validate and fix scroll divider index
    if (trackball_config.scrl_div >= SCRL_DIV_SIZE) {
        trackball_config.scrl_div = TRACKBALL_SCROLL_DIV_DEFAULT;
        needs_save = true;
    }
    // Validate and fix rotation angle index
    if (trackball_config.rotation_angle >= ANGLE_SIZE) {
        trackball_config.rotation_angle = TRACKBALL_ROTATION_DEFAULT;
        needs_save = true;
    }
    // Validate and fix adaptive gain (check both bounds and uninitialized values)
    // Check if out of valid range [MIN, MAX] or uninitialized (0xFF when reserved is valid)
    // Note: 0x00 is a valid value (equals MIN), so we only check for 0xFF as uninitialized
    if (trackball_config.adaptive_gain < TRACKBALL_ADAPTIVE_GAIN_SETTING_MIN || 
        trackball_config.adaptive_gain > TRACKBALL_ADAPTIVE_GAIN_SETTING_MAX ||
        (trackball_config.reserved == TRACKBALL_CONFIG_VERSION && trackball_config.adaptive_gain == 0xFF)) {
#ifdef CONSOLE_ENABLE
        xprintf("GAIN[INIT] Invalid value detected: %u, resetting to default: %u\n",
                trackball_config.adaptive_gain, TRACKBALL_ADAPTIVE_GAIN_SETTING_DEFAULT);
#endif
        trackball_config.adaptive_gain = TRACKBALL_ADAPTIVE_GAIN_SETTING_DEFAULT;
        needs_save = true;
    }
#ifdef CONSOLE_ENABLE
    else {
        xprintf("GAIN[INIT] Valid value loaded: %u (range: %u-%u)\n",
                trackball_config.adaptive_gain, 
                TRACKBALL_ADAPTIVE_GAIN_SETTING_MIN,
                TRACKBALL_ADAPTIVE_GAIN_SETTING_MAX);
    }
#endif

    pointing_device_set_cpi(cpi_array[trackball_config.cpi_idx]);
    trackball_refresh_rotation_cache();

    // Save config if any validation fixes were applied to ensure persistence
    if (needs_save) {
#ifdef CONSOLE_ENABLE
        xprintf("GAIN[INIT] Saving config: adaptive_gain=%u reserved=%u raw=0x%08lx\n",
                trackball_config.adaptive_gain, trackball_config.reserved,
                (unsigned long)trackball_config_pack(&trackball_config));
#endif
        eeconfig_update_kb(trackball_config_pack(&trackball_config));
        wait_ms(2);// ensure flash commit before unplug
#ifdef CONSOLE_ENABLE
        // Verify save
        trackball_config_t verify;
        uint32_t verify_raw = eeconfig_read_kb();
    trackball_config_unpack(verify_raw, &verify);
        xprintf("GAIN[INIT] After save: read gain=%u reserved=%u\n", verify.adaptive_gain, verify.reserved);
        if (verify.adaptive_gain != trackball_config.adaptive_gain || verify.reserved != trackball_config.reserved) {
            xprintf("GAIN[INIT] ERROR: Save verification failed! Written gain=%u reserved=%u, Read gain=%u reserved=%u\n",
                    trackball_config.adaptive_gain, trackball_config.reserved,
                    verify.adaptive_gain, verify.reserved);
        }
#endif
    }

    set_auto_mouse_enable(trackball_config.auto_mouse);
    log_scroll_status("init");
    trackball_emit_gain_status("INIT");
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    float sensitivity = TRACKBALL_MOVEMENT_SENSITIVITY;
    float sensitivity_multiplier = TRACKBALL_SENSITIVITY_MULTIPLIER;

    float rotated_x = -(mouse_report.x * trackball_state.rotation_cos - mouse_report.y * trackball_state.rotation_sin);
    float rotated_y = mouse_report.x * trackball_state.rotation_sin + mouse_report.y * trackball_state.rotation_cos;

    trackball_history_push(rotated_x, rotated_y, timer_read32());

    float cursor_smoothed_x = trackball_apply_smoothing(&trackball_state.cursor_prev_x, rotated_x, TRACKBALL_CURSOR_SMOOTHING);
    float cursor_smoothed_y = trackball_apply_smoothing(&trackball_state.cursor_prev_y, rotated_y, TRACKBALL_CURSOR_SMOOTHING);

    trackball_mode_t active_mode = trackball_resolve_mode();
    trackball_handle_mode_transition(active_mode, rotated_x, rotated_y);

    float mode_smoothed_x = trackball_apply_smoothing(&trackball_state.mode_prev_x, rotated_x, TRACKBALL_MODE_SMOOTHING);
    float mode_smoothed_y = trackball_apply_smoothing(&trackball_state.mode_prev_y, rotated_y, TRACKBALL_MODE_SMOOTHING);

    bool suppress_cursor_motion = false;
    if (active_mode == TRACKBALL_MODE_CURSOR) {
        suppress_cursor_motion = trackball_typing_guard_should_suppress(cursor_smoothed_x, cursor_smoothed_y);
        if (suppress_cursor_motion) {
            cursor_smoothed_x = 0.0f;
            cursor_smoothed_y = 0.0f;
        }
    } else {
        trackball_state.typing_guard_active = false;
    }

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

    float adaptive_gain = trackball_compute_adaptive_gain();
    cursor_smoothed_x *= adaptive_gain;
    cursor_smoothed_y *= adaptive_gain;

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
#ifdef CONSOLE_ENABLE
#if TRACKBALL_SCROLL_DEBUG
        // Only log when SCRL_MO is actually active (not layer-based scroll mode)
        if (scrl_mo_active) {
            static uint32_t last_scroll_log_time = 0;
            uint32_t now = timer_read32();
            // Log entry into scroll mode every 3 seconds to verify it's active
            if (timer_elapsed32(last_scroll_log_time) > 3000) {
                xprintf("SCRL_DEBUG[ACTIVE] Scroll mode active via SCRL_MO (mo=%u toggle=%u)\n",
                        scrl_mo_active ? 1 : 0, trackball_config.scrl_mode ? 1 : 0);
                last_scroll_log_time = now;
            }
        }
#endif
#endif
        float scroll_x = mode_smoothed_x;
        float scroll_y = mode_smoothed_y;

#ifdef CONSOLE_ENABLE
#if TRACKBALL_SCROLL_DEBUG
        float raw_x = rotated_x;
        float raw_y = rotated_y;
#endif
#endif

        if (fabsf(scroll_x) > fabsf(scroll_y)) {
            scroll_y = 0.0f;
        } else {
            scroll_x = 0.0f;
        }

        scroll_x = trackball_apply_deadband(scroll_x, TRACKBALL_MODE_DEADBAND);
        scroll_y = trackball_apply_deadband(scroll_y, TRACKBALL_MODE_DEADBAND);

#ifdef CONSOLE_ENABLE
#if TRACKBALL_SCROLL_DEBUG
        // Only log movement/accumulator when SCRL_MO is active (not layer-based scroll)
        if (scrl_mo_active) {
            log_scroll_debug_movement(raw_x, raw_y, mode_smoothed_x, mode_smoothed_y, scroll_x, scroll_y);
        }
#endif
#endif

        float horiz_factor = trackball_config.scrl_inv ? 1.0f : -1.0f;
        float vert_factor = trackball_config.scrl_inv ? -1.0f : 1.0f;
        trackball_state.scroll_accum_x += horiz_factor * scroll_x * TRACKBALL_SCROLL_SENSITIVITY;
        trackball_state.scroll_accum_y += vert_factor * scroll_y * TRACKBALL_SCROLL_SENSITIVITY;

        float scroll_divisor = (float)scrl_div_array[trackball_config.scrl_div];
        
#ifdef CONSOLE_ENABLE
#if TRACKBALL_SCROLL_DEBUG
        // Only log accumulator when SCRL_MO is active (not layer-based scroll)
        if (scrl_mo_active) {
            log_scroll_debug_accumulator(trackball_state.scroll_accum_x, trackball_state.scroll_accum_y, scroll_divisor);
        }
#endif
#endif

        int8_t h_scroll = 0;
        if (fabsf(trackball_state.scroll_accum_x) >= scroll_divisor) {
            float steps = trackball_state.scroll_accum_x / scroll_divisor;
            steps = fmaxf(-127.0f, fminf(127.0f, steps));
            h_scroll = (int8_t)steps;
            trackball_state.scroll_accum_x -= (float)h_scroll * scroll_divisor;
        } else {
#ifdef CONSOLE_ENABLE
#if TRACKBALL_SCROLL_DEBUG
            // Only log no-event when SCRL_MO is active
            if (scrl_mo_active) {
                log_scroll_debug_no_event("h_accum_below_divisor", trackball_state.scroll_accum_x, trackball_state.scroll_accum_y, scroll_divisor);
            }
#endif
#endif
        }

        int8_t v_scroll = 0;
        if (fabsf(trackball_state.scroll_accum_y) >= scroll_divisor) {
            float steps = trackball_state.scroll_accum_y / scroll_divisor;
            steps = fmaxf(-127.0f, fminf(127.0f, steps));
            v_scroll = (int8_t)steps;
            trackball_state.scroll_accum_y -= (float)v_scroll * scroll_divisor;
        } else {
#ifdef CONSOLE_ENABLE
#if TRACKBALL_SCROLL_DEBUG
            // Only log no-event when SCRL_MO is active
            if (scrl_mo_active && h_scroll == 0) {  // Only log if no horizontal scroll either
                log_scroll_debug_no_event("v_accum_below_divisor", trackball_state.scroll_accum_x, trackball_state.scroll_accum_y, scroll_divisor);
            }
#endif
#endif
        }

#ifdef CONSOLE_ENABLE
#if TRACKBALL_SCROLL_DEBUG
        // Always log events (they're important), but only when SCRL_MO is active
        if (scrl_mo_active && (h_scroll != 0 || v_scroll != 0)) {
            log_scroll_debug_event(h_scroll, v_scroll, trackball_state.scroll_accum_x, trackball_state.scroll_accum_y);
        }
#endif
#endif

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

    trackball_note_typing_event(keycode, record);
    
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
        eeconfig_update_kb(trackball_config_pack(&trackball_config));
        wait_ms(2);// ensure flash commit before unplug
        
        // Verify the value was saved correctly
        #ifdef CONSOLE_ENABLE
        trackball_config_t verify_config;
        uint32_t verify_raw = eeconfig_read_kb();
            trackball_config_unpack(verify_raw, &verify_config);
        xprintf("CPI_SW verify: saved_idx=%u, current_idx=%u\n", verify_config.cpi_idx, trackball_config.cpi_idx);
        #endif
        
        return false; // Prevent further processing
    }

    if (keycode == GAIN_UP && record->event.pressed) {
        // Reload config from EEPROM first to ensure we have latest values (like CPI_SW might implicitly do)
        trackball_config_unpack(eeconfig_read_kb(), &trackball_config);
        
        int16_t value = (int16_t)trackball_config.adaptive_gain + TRACKBALL_ADAPTIVE_GAIN_SETTING_STEP;
        if (value > TRACKBALL_ADAPTIVE_GAIN_SETTING_MAX) {
            value = TRACKBALL_ADAPTIVE_GAIN_SETTING_MAX;
        }
        if (value != trackball_config.adaptive_gain) {
            uint8_t old_gain = trackball_config.adaptive_gain;
            trackball_config.adaptive_gain = (uint8_t)value;
            
            // Ensure reserved is set
            if (trackball_config.reserved != TRACKBALL_CONFIG_VERSION) {
                trackball_config.reserved = TRACKBALL_CONFIG_VERSION;
            }
            
            #ifdef CONSOLE_ENABLE
            xprintf("GAIN[SAVE] Before save: gain=%u->%u reserved=%u raw=0x%08lx\n",
                    old_gain, trackball_config.adaptive_gain, trackball_config.reserved,
                    (unsigned long)trackball_config_pack(&trackball_config));
            #endif
            
            // Save to EEPROM - exactly same way as CPI_SW does it
            eeconfig_update_kb(trackball_config_pack(&trackball_config));
        wait_ms(2);// ensure flash commit before unplug
            
            #ifdef CONSOLE_ENABLE
            // Verify the value was saved correctly - same way as CPI_SW does it
            trackball_config_t verify_config;
            uint32_t verify_raw = eeconfig_read_kb();
            trackball_config_unpack(verify_raw, &verify_config);
            xprintf("GAIN[SAVE] After save: read gain=%u reserved=%u\n",
                    verify_config.adaptive_gain, verify_config.reserved);
            if (verify_config.adaptive_gain != trackball_config.adaptive_gain) {
                xprintf("GAIN[ERROR] Mismatch! Written=%u Read=%u\n", trackball_config.adaptive_gain, verify_config.adaptive_gain);
            }
            #endif
            
            trackball_emit_gain_status("GAIN_UP");
        }
        return false;
    }
    if (keycode == GAIN_DN && record->event.pressed) {
        // Reload config from EEPROM first to ensure we have latest values (like CPI_SW might implicitly do)
        trackball_config_unpack(eeconfig_read_kb(), &trackball_config);
        
        int16_t value = (int16_t)trackball_config.adaptive_gain - TRACKBALL_ADAPTIVE_GAIN_SETTING_STEP;
        if (value < TRACKBALL_ADAPTIVE_GAIN_SETTING_MIN) {
            value = TRACKBALL_ADAPTIVE_GAIN_SETTING_MIN;
        }
        if (value != trackball_config.adaptive_gain) {
            uint8_t old_gain = trackball_config.adaptive_gain;
            trackball_config.adaptive_gain = (uint8_t)value;
            
            // Ensure reserved is set
            if (trackball_config.reserved != TRACKBALL_CONFIG_VERSION) {
                trackball_config.reserved = TRACKBALL_CONFIG_VERSION;
            }
            
            #ifdef CONSOLE_ENABLE
            xprintf("GAIN[SAVE] Before save: gain=%u->%u reserved=%u raw=0x%08lx\n",
                    old_gain, trackball_config.adaptive_gain, trackball_config.reserved,
                    (unsigned long)trackball_config_pack(&trackball_config));
            #endif
            
            // Save to EEPROM - exactly same way as CPI_SW does it
            eeconfig_update_kb(trackball_config_pack(&trackball_config));
        wait_ms(2);// ensure flash commit before unplug
            
            #ifdef CONSOLE_ENABLE
            // Verify the value was saved correctly - same way as CPI_SW does it
            trackball_config_t verify_config;
            uint32_t verify_raw = eeconfig_read_kb();
            trackball_config_unpack(verify_raw, &verify_config);
            xprintf("GAIN[SAVE] After save: read gain=%u reserved=%u\n",
                    verify_config.adaptive_gain, verify_config.reserved);
            if (verify_config.adaptive_gain != trackball_config.adaptive_gain) {
                xprintf("GAIN[ERROR] Mismatch! Written=%u Read=%u\n", trackball_config.adaptive_gain, verify_config.adaptive_gain);
            }
            #endif
            
            trackball_emit_gain_status("GAIN_DN");
        }
        return false;
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
                eeconfig_update_kb(trackball_config_pack(&trackball_config));
        wait_ms(2);// ensure flash commit before unplug
                set_auto_mouse_enable(trackball_config.auto_mouse);
                //auto_mouse_tg_off = !get_auto_mouse_enable();
            } // do nothing on key up
            return false; // prevent further processing of keycode            
    //*/
    }

    if (keycode == SCRL_SW && record->event.pressed) {
        trackball_config.scrl_div = (trackball_config.scrl_div + 1) % SCRL_DIV_SIZE;
        eeconfig_update_kb(trackball_config_pack(&trackball_config));
        wait_ms(2);// ensure flash commit before unplug
        log_scroll_status("SCRL_SW");
        return false;
    }

    if (keycode == ROT_R15 && record->event.pressed) {
        trackball_config.rotation_angle = (trackball_config.rotation_angle + 1) % ANGLE_SIZE;
        eeconfig_update_kb(trackball_config_pack(&trackball_config));
        wait_ms(2);// ensure flash commit before unplug
        trackball_refresh_rotation_cache();
        return false;
    }

    if (keycode == ROT_L15 && record->event.pressed) {
        trackball_config.rotation_angle = (ANGLE_SIZE + trackball_config.rotation_angle - 1) % ANGLE_SIZE;
        eeconfig_update_kb(trackball_config_pack(&trackball_config));
        wait_ms(2);// ensure flash commit before unplug
        trackball_refresh_rotation_cache();
        return false;
    }

    if (keycode == SCRL_IN && record->event.pressed) {
        trackball_config.scrl_inv ^= 1;
        eeconfig_update_kb(trackball_config_pack(&trackball_config));
        wait_ms(2);// ensure flash commit before unplug
        log_scroll_status("SCRL_IN");
        return false;
    }

    if (keycode == SCRL_TO && record->event.pressed) {
        trackball_set_scroll_mode(!trackball_config.scrl_mode);
        eeconfig_update_kb(trackball_config_pack(&trackball_config));
        wait_ms(2);// ensure flash commit before unplug
        log_scroll_status("SCRL_TO");
        return false;
    }

    if (keycode == SCRL_MO) {
        scrl_mo_active = record->event.pressed;
        log_scroll_status(scrl_mo_active ? "SCRL_MO(on)" : "SCRL_MO(off)");
#ifdef CONSOLE_ENABLE
#if TRACKBALL_SCROLL_DEBUG
        trackball_mode_t current_mode = trackball_resolve_mode();
        xprintf("SCRL_DEBUG[KEY] SCRL_MO %s -> mode=%s (mo=%u toggle=%u)\n",
                record->event.pressed ? "PRESSED" : "RELEASED",
                trackball_mode_name(current_mode),
                scrl_mo_active ? 1 : 0,
                trackball_config.scrl_mode ? 1 : 0);
#endif
#endif
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
            // Note: Scroll mode is NOT activated by layers - only by SCRL_MO key
            state = remove_auto_mouse_layer(state, false);
            set_auto_mouse_enable(false);
            break;
        case 3 ... 7:
            //rgblight_sethsv_range(HSV_CYAN, 0, 9);
            //set_auto_mouse_enable(true);
            break;
        default:
            //rgblight_sethsv_range(HSV_RED, 0, 9);
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
    trackball_config_t existing;
    trackball_config_unpack(eeconfig_read_kb(), &existing);
    if (existing.reserved == TRACKBALL_CONFIG_VERSION) {
#ifdef CONSOLE_ENABLE
        xprintf("GAIN[EEINIT] Preserve existing config: gain=%u reserved=%u raw=0x%08lx\n",
                existing.adaptive_gain, existing.reserved,
                (unsigned long)trackball_config_pack(&existing));
#endif
        trackball_config = existing;
        trackball_refresh_rotation_cache();
        eeconfig_init_user();
        return;
    }

    trackball_config.cpi_idx = TRACKBALL_CPI_DEFAULT;
    trackball_config.scrl_div = TRACKBALL_SCROLL_DIV_DEFAULT;
    trackball_config.rotation_angle = TRACKBALL_ROTATION_DEFAULT;
    trackball_config.scrl_inv = TRACKBALL_SCROLL_INV_DEFAULT;
    trackball_config.scrl_mode = false;
    trackball_config.auto_mouse = TRACKBALL_AUTO_MOUSE_MODE;
    trackball_config.adaptive_gain = TRACKBALL_ADAPTIVE_GAIN_SETTING_DEFAULT;
    trackball_config.reserved = TRACKBALL_CONFIG_VERSION;
    scrl_mo_active = false;
    vol_mo_active = false;
    vol_toggle_active = false;
    brt_mo_active = false;
    brt_toggle_active = false;
    trackball_refresh_rotation_cache();
    
#ifdef CONSOLE_ENABLE
    xprintf("GAIN[EEINIT] Before save: gain=%u reserved=%u raw=0x%08lx\n",
            trackball_config.adaptive_gain, trackball_config.reserved,
            (unsigned long)trackball_config_pack(&trackball_config));
#endif
    
    eeconfig_update_kb(trackball_config_pack(&trackball_config));
        wait_ms(2);// ensure flash commit before unplug
    
#ifdef CONSOLE_ENABLE
    // Verify the save worked
    trackball_config_t verify;
    uint32_t verify_raw = eeconfig_read_kb();
    trackball_config_unpack(verify_raw, &verify);
    xprintf("GAIN[EEINIT] After save: read gain=%u reserved=%u raw=0x%08lx\n",
            verify.adaptive_gain, verify.reserved, (unsigned long)trackball_config_pack(&verify));
    if (verify.reserved != TRACKBALL_CONFIG_VERSION) {
        xprintf("GAIN[EEINIT] ERROR: Save failed! reserved written=%u read=%u\n",
                TRACKBALL_CONFIG_VERSION, verify.reserved);
    }
#endif
    
    eeconfig_init_user();
}


void matrix_init_kb(void) {
    // is safe to just read CPI setting since matrix init
    // comes before pointing device init.
    trackball_config_unpack(eeconfig_read_kb(), &trackball_config);
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

