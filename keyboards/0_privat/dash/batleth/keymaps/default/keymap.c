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

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * LAYOUT order (36 switches) — right half first, then left, per row.
     * Matrix: 8 rows x 10 cols (rows 0–3 / cols 0–4 left, rows 4–7 / cols 5–9 right).
     */
    [_BASE] = LAYOUT(
        KC_Y,    KC_U,    KC_I,    KC_O,    KC_BSPC, KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,
        KC_H,    KC_J,    KC_K,    KC_L,    KC_ENT,  KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,
        KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SCLN, KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,
        KC_LCTL, KC_LGUI, KC_SPC,  KC_LALT, KC_RGUI, KC_RSFT
    ),
    [_LOWER] = LAYOUT(
        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,
        KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS, KC_ESC,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,
        KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_GRV,  _______, KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR,
        _______, _______, _______, _______, _______, _______
    ),
    [_RAISE] = LAYOUT(
        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,
        KC_HOME, KC_PGUP, KC_PGDN, KC_END,  KC_DEL,  KC_CAPS, KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC,
        KC_VOLD, KC_VOLU, KC_MUTE, KC_PIPE, KC_TILD, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT,
        _______, _______, _______, _______, _______, _______
    ),
    [_ADJUST] = LAYOUT(
        _______, _______, _______, _______, QK_BOOT, QK_BOOT, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______
    ),
};
