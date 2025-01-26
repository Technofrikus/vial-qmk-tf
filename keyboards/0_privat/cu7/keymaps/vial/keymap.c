/* Copyright 2021 CapsUnlocked
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
#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT(
             MO(1),
    KC_1,    KC_2,    KC_3,
    KC_4,    KC_5,    KC_6
  ),
  [1] = LAYOUT(
             _______,
    RGB_MOD, KC_UP,   QK_BOOT,
    KC_LEFT, KC_DOWN, KC_RGHT
  )
};

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = { ENCODER_CCW_CW(KC_NO, KC_NO) },
    [1] = { ENCODER_CCW_CW(KC_NO, KC_NO) }
};

layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case 0: // Default Layer
            rgblight_sethsv_noeeprom(0, 0, 255); // Weiß
            break;
        case 1: // Layer 1
            rgblight_sethsv_noeeprom(8, 255, 255); // Rotorange
            break;
        case 2: // Layer 2
            rgblight_sethsv_noeeprom(100, 255, 255); // Grün
            break;
        default: // Fallback für andere Layer
            rgblight_sethsv_noeeprom(0, 0, 0); // LEDs aus
            break;
    }
    return state;
}

void keyboard_post_init_user(void) {
    // Setze Layer 0 aktiv und aktualisiere die RGB-Farbe
    layer_state_set_user(0);
}
