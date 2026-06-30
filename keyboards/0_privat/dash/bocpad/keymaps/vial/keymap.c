// Copyright 2026 Technofrikus
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

enum layers {
    _BASE,
    _LOWER,
    _RAISE,
    _ADJUST,
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * LAYOUT order matches keyboard.json / vial.json (KLE export order).
     * Matrix cols 0..6: B0, A5, B1, A6, A3, A7, A4
     */
    [_BASE] = LAYOUT(
        KC_1,    KC_2,    KC_3,
        KC_4,    KC_5,    KC_6,
        KC_7
    ),
    [_LOWER] = LAYOUT(
        KC_F1,   KC_F2,   KC_F3,
        KC_F4,   KC_F5,   KC_F6,
        KC_F6
    ),
    [_RAISE] = LAYOUT(
        KC_MINS, KC_EQL,  KC_RBRC,
        KC_LBRC, KC_BSLS, KC_QUOT,
        KC_SCLN
    ),
    [_ADJUST] = LAYOUT(
        _______, _______, _______,
        _______, QK_BOOT, _______,
        _______
    ),
};
