/* Copyright 2023 projectcain
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
#include "rgb_matrix.h"
#include "matrix.h"
#include <lib/lib8tion/lib8tion.h>

#ifdef RGB_MATRIX_KEYREACTIVE_ENABLED
// Lookup table mapping matrix positions to all LED indices
// This table is based on the LED layout in keyboard.json where multiple LEDs
// can be mapped to the same key position
// LED indices correspond to the position in the rgb_matrix.layout array
static const uint8_t key_to_leds[MATRIX_ROWS][MATRIX_COLS][LED_HITS_TO_REMEMBER] = {
    // Row 0 - alle Tasten sind Edge (oben), haben LEDs
    [0] = {
        [0] = {2, 3, 4, 5, NO_LED, NO_LED, NO_LED, NO_LED},  // [0,0] - LEDs at indices 2,3,4,5
        [1] = {6, 7, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [0,1] - LEDs at indices 6,7
        [2] = {8, 9, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [0,2] - LEDs at indices 8,9
        [3] = {10, 11, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [0,3] - LEDs at indices 10,11
        [4] = {12, 13, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [0,4] - LEDs at indices 12,13
        [5] = {14, 15, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [0,5] - LEDs at indices 14,15
        [6] = {16, 17, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [0,6] - LEDs at indices 16,17
        [7] = {18, 19, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [0,7] - LEDs at indices 18,19
        [8] = {20, 21, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [0,8] - LEDs at indices 20,21
        [9] = {22, 23, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [0,9] - LEDs at indices 22,23
        [10] = {24, 25, 26, 27, NO_LED, NO_LED, NO_LED, NO_LED},  // [0,10] - LEDs at indices 24,25,26,27
    },
    // Row 1 - nur Edge-Tasten haben LEDs (links und rechts)
    [1] = {
        [0] = {0, 1, 61, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [1,0] - LEDs at indices 0,1,60,61 (links)
        [1] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [1,1] - keine LEDs
        [2] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [1,2] - keine LEDs
        [3] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [1,3] - keine LEDs
        [4] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [1,4] - keine LEDs
        [5] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [1,5] - keine LEDs
        [6] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [1,6] - keine LEDs
        [7] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [1,7] - keine LEDs
        [8] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [1,8] - keine LEDs
        [9] = {28, 29, 30, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [1,9] - LEDs at indices 28,29,30 (rechts)
    },
    // Row 2 - nur Edge-Tasten haben LEDs (links und rechts)
    [2] = {
        [0] = {55, 56, 57, 58, 59, NO_LED, NO_LED, NO_LED},  // [2,0] - LEDs at indices 55,56,57,58,59 (links)
        [1] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [2,1] - keine LEDs
        [2] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [2,2] - keine LEDs
        [3] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [2,3] - keine LEDs
        [4] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [2,4] - keine LEDs
        [5] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [2,5] - keine LEDs
        [6] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [2,6] - keine LEDs
        [7] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [2,7] - keine LEDs
        [8] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [2,8] - keine LEDs
        [9] = {31, 32, 33, 34, 35, NO_LED, NO_LED, NO_LED},  // [2,9] - LEDs at indices 31,32,33,34,35 (rechts)
    },
    // Row 3 - alle Tasten sind Edge (unten), haben LEDs
    [3] = {
        [0] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [3,0] - keine LEDs (existiert nicht in Layout)
        [1] = {52, 53, 54, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [3,1] - LEDs at indices 52,53,54
        [2] = {50, 51, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [3,2] - LEDs at indices 50,51
        [3] = {46, 47, 48, 49, NO_LED, NO_LED, NO_LED, NO_LED},  // [3,3] - LEDs at indices 46,47,48,49
        [4] = {NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [3,4] - keine LEDs (existiert nicht in Layout)
        [5] = {42, 43, 44, 45, NO_LED, NO_LED, NO_LED, NO_LED},  // [3,5] - LEDs at indices 42,43,44,45
        [6] = {40, 41, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [3,6] - LEDs at indices 40,41
        [7] = {37, 38, 39, NO_LED, NO_LED, NO_LED, NO_LED, NO_LED},  // [3,7] - LEDs at indices 37,38,39
    },
};

// Override rgb_matrix_map_row_column_to_led_kb to return all LEDs mapped to a key
// This allows multiple LEDs per key to react to key presses
uint8_t rgb_matrix_map_row_column_to_led_kb(uint8_t row, uint8_t column, uint8_t *led_i) {
    uint8_t led_count = 0;
    
    // Check if row/column is valid
    if (row >= MATRIX_ROWS || column >= MATRIX_COLS) {
        return 0;
    }
    
    // Check if this key has any LEDs mapped (first entry must not be NO_LED)
    // Note: LED index 0 is valid, so we only check for NO_LED
    uint8_t first_led = key_to_leds[row][column][0];
    if (first_led == NO_LED) {
        return 0; // No LEDs for this key
    }
    
    // Look up all LEDs for this key position
    for (uint8_t i = 0; i < LED_HITS_TO_REMEMBER; i++) {
        uint8_t led_idx = key_to_leds[row][column][i];
        if (led_idx == NO_LED || led_idx >= RGB_MATRIX_LED_COUNT) {
            break; // End of list for this key
        }
        led_i[led_count++] = led_idx;
    }
    
    return led_count;
}
#endif // RGB_MATRIX_KEYREACTIVE_ENABLED

void keyboard_post_init_kb(void) {
    // Disable debug output to reduce USB overhead and improve stability
    debug_enable   = false;
    debug_keyboard = false;

    keyboard_post_init_user();
}
