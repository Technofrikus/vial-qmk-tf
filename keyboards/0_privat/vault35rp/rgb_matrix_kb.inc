RGB_MATRIX_EFFECT(SOLID_KEYPRESS)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#include "matrix.h"

#define SOLID_KEYPRESS_FADE_IN_TIME_MS 50      // Fast fade in (50ms)
#define SOLID_KEYPRESS_HOLD_TIME_MS 1000        // Stay on for 1s after release
#define SOLID_KEYPRESS_FADE_OUT_TIME_MS 300    // Fade out duration (300ms)

static bool SOLID_KEYPRESS(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // Turn off all LEDs first
    if (params->init) {
        rgb_matrix_set_color_all(0, 0, 0);
    }

    // Get the HSV color from config (configurable via VIAL)
    hsv_t hsv = rgb_matrix_config.hsv;
    rgb_t rgb_full = hsv_to_rgb(hsv);
    
    // Track brightness for each LED (0 = off, 255 = full brightness)
    uint8_t led_brightness[RGB_MATRIX_LED_COUNT] = {0};
    
    // First, process all tracked LED hits (for fade out after release)
    for (uint8_t i = 0; i < g_last_hit_tracker.count; i++) {
        uint16_t tick = g_last_hit_tracker.tick[i];
        uint8_t led_idx = g_last_hit_tracker.index[i];
        
        if (led_idx >= led_min && led_idx < led_max) {
            // Check if key is currently pressed
            bool key_pressed = false;
            for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
                for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                    uint8_t key_leds[LED_HITS_TO_REMEMBER];
                    uint8_t led_count = rgb_matrix_map_row_column_to_led(row, col, key_leds);
                    for (uint8_t j = 0; j < led_count; j++) {
                        if (key_leds[j] == led_idx && matrix_is_on(row, col)) {
                            key_pressed = true;
                            break;
                        }
                    }
                    if (key_pressed) break;
                }
                if (key_pressed) break;
            }
            
            uint8_t brightness = 0;
            if (key_pressed) {
                // Key is pressed: fade in quickly or full brightness
                if (tick < SOLID_KEYPRESS_FADE_IN_TIME_MS) {
                    brightness = scale8(255, (uint8_t)(tick * 255 / SOLID_KEYPRESS_FADE_IN_TIME_MS));
                } else {
                    brightness = 255; // Full brightness while held
                }
            } else {
                // Key is released: stay on for 1s, then fade out
                if (tick < SOLID_KEYPRESS_HOLD_TIME_MS) {
                    brightness = 255; // Full brightness during hold time
                } else if (tick < SOLID_KEYPRESS_HOLD_TIME_MS + SOLID_KEYPRESS_FADE_OUT_TIME_MS) {
                    // Fade out
                    uint16_t fade_time = tick - SOLID_KEYPRESS_HOLD_TIME_MS;
                    brightness = qsub8(255, scale8(255, (uint8_t)(fade_time * 255 / SOLID_KEYPRESS_FADE_OUT_TIME_MS)));
                } else {
                    brightness = 0; // Fully off after fade out
                }
            }
            
            // Store the maximum brightness for this LED (in case multiple keys share the same LED)
            if (brightness > led_brightness[led_idx]) {
                led_brightness[led_idx] = brightness;
            }
        }
    }
    
    // Second, process all currently pressed keys (to ensure they light up even if not in tracker yet)
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            if (matrix_is_on(row, col)) {
                uint8_t key_leds[LED_HITS_TO_REMEMBER];
                uint8_t led_count = rgb_matrix_map_row_column_to_led(row, col, key_leds);
                
                // Find the most recent tick for this key
                uint16_t min_tick = UINT16_MAX;
                for (uint8_t i = 0; i < g_last_hit_tracker.count; i++) {
                    for (uint8_t j = 0; j < led_count; j++) {
                        if (g_last_hit_tracker.index[i] == key_leds[j] && g_last_hit_tracker.tick[i] < min_tick) {
                            min_tick = g_last_hit_tracker.tick[i];
                        }
                    }
                }
                
                // If not found in tracker, assume it's just pressed (tick = 0)
                if (min_tick == UINT16_MAX) {
                    min_tick = 0;
                }
                
                // Calculate brightness for all LEDs of this key
                uint8_t brightness = 255;
                if (min_tick < SOLID_KEYPRESS_FADE_IN_TIME_MS) {
                    brightness = scale8(255, (uint8_t)(min_tick * 255 / SOLID_KEYPRESS_FADE_IN_TIME_MS));
                }
                
                // Apply to all LEDs of this key
                for (uint8_t i = 0; i < led_count; i++) {
                    uint8_t led_idx = key_leds[i];
                    if (led_idx >= led_min && led_idx < led_max) {
                        if (brightness > led_brightness[led_idx]) {
                            led_brightness[led_idx] = brightness;
                        }
                    }
                }
            }
        }
    }
    
    // Finally, apply all LED brightnesses
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





















