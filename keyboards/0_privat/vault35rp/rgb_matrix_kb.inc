RGB_MATRIX_EFFECT(SOLID_KEYPRESS)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#include "matrix.h"

#define SOLID_KEYPRESS_FADE_IN_TIME_MS 50      // Fast fade in (50ms)
#define SOLID_KEYPRESS_HOLD_TIME_MS 1000        // Stay on for 1s after release
#define SOLID_KEYPRESS_FADE_OUT_TIME_MS 300    // Fade out duration (300ms)

// Static brightness array to avoid stack allocation
static uint8_t led_brightness[RGB_MATRIX_LED_COUNT];

static bool SOLID_KEYPRESS(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // Turn off all LEDs first
    if (params->init) {
        rgb_matrix_set_color_all(0, 0, 0);
        // Clear brightness array on init
        memset(led_brightness, 0, sizeof(led_brightness));
    }

    // Get the HSV color from config (configurable via VIAL)
    hsv_t hsv = rgb_matrix_config.hsv;
    rgb_t rgb_full = hsv_to_rgb(hsv);
    
    // Clear brightness array for this frame
    memset(led_brightness, 0, sizeof(led_brightness));
    
    // Process all tracked LED hits (for fade out after release)
    // This is much more efficient than checking the entire matrix
    for (uint8_t i = 0; i < g_last_hit_tracker.count; i++) {
        uint16_t tick = g_last_hit_tracker.tick[i];
        uint8_t led_idx = g_last_hit_tracker.index[i];
        
        if (led_idx >= led_min && led_idx < led_max) {
            uint8_t brightness = 0;
            
            // Calculate brightness based on time since key press
            if (tick < SOLID_KEYPRESS_FADE_IN_TIME_MS) {
                // Fade in
                brightness = scale8(255, (uint8_t)((tick * 255) / SOLID_KEYPRESS_FADE_IN_TIME_MS));
            } else if (tick < SOLID_KEYPRESS_HOLD_TIME_MS) {
                // Full brightness during hold time
                brightness = 255;
            } else if (tick < SOLID_KEYPRESS_HOLD_TIME_MS + SOLID_KEYPRESS_FADE_OUT_TIME_MS) {
                // Fade out
                uint16_t fade_time = tick - SOLID_KEYPRESS_HOLD_TIME_MS;
                brightness = qsub8(255, scale8(255, (uint8_t)((fade_time * 255) / SOLID_KEYPRESS_FADE_OUT_TIME_MS)));
            }
            // else brightness stays 0 (fully off)
            
            // Store the maximum brightness for this LED (in case multiple keys share the same LED)
            if (brightness > led_brightness[led_idx]) {
                led_brightness[led_idx] = brightness;
            }
        }
    }
    
    // Apply all LED brightnesses
    for (uint8_t i = led_min; i < led_max; i++) {
        if (led_brightness[i] > 0) {
            rgb_t rgb = {
                .r = scale8(rgb_full.r, led_brightness[i]),
                .g = scale8(rgb_full.g, led_brightness[i]),
                .b = scale8(rgb_full.b, led_brightness[i])
            };
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        } else {
            rgb_matrix_set_color(i, 0, 0, 0);
        }
    }

    return rgb_matrix_check_finished_leds(led_max);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS





















