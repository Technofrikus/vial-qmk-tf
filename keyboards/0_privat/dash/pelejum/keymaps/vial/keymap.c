// Copyright 2026
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
#define ADJUST MO(_ADJUST)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * LAYOUT order (42 switches) — matches keyboard.json / vial.json matrix positions.
     * vial.json layout options (LAE, bottom row) only change the Vial UI; all positions
     * stay in this keymap. Default: LAE off, 6.25u bottom (3,0/3,1/3,5 + 3,9–3,11).
     */
    [_BASE] = LAYOUT(
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_BSPC,
        RAISE,   KC_ENT,
        KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,
        LOWER,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,
        KC_NO,   KC_DOT,
        KC_LCTL, KC_LGUI, KC_SPC,  KC_LALT, KC_RGUI, KC_RSFT,
        KC_NO,   KC_NO,   KC_NO
    ),
    [_LOWER] = LAYOUT(
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
        _______, _______,
        KC_ESC,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR,
        _______,
        _______, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD, KC_GRV,  KC_SCLN,
        _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______
    ),
    [_RAISE] = LAYOUT(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_DEL,
        _______, _______,
        KC_CAPS, KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS, KC_HOME, KC_UP,   KC_END,
        _______,
        _______, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGUP, KC_PGDN, KC_VOLD, KC_VOLU, KC_MUTE,
        _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______
    ),
    [_ADJUST] = LAYOUT(
        QK_BOOT, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______,
        _______, _______, _______, _______, _______, _______,
        _______, _______, _______
};
