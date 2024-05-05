// Copyright 2024 LAZDESIGNERS
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "keymap_german_mac_iso.h"

#define LT2_TAB LT(2, KC_TAB)
#define LT1_SPC LT(1, KC_SPC)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_ESC,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
        LT2_TAB,   KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_ENT,
        KC_LSFT,   KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_DOT,  KC_UP,   KC_COMM, MO(2),
        KC_LCTL,   KC_LGUI, KC_RALT,          LT1_SPC,          KC_SPC,  KC_SPC,  KC_LEFT, KC_DOWN, KC_RGHT
        ),
    [1] = LAYOUT(
        DE_DEG,    DE_CIRC,    DE_LCBR,    DE_RCBR,    DE_DLR,    DE_DQUO,    DE_UDIA,    DE_AMPR,    DE_ODIA,    DE_SECT,    KC_0,    KC_NO,
        QK_BOOT,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_NO,
        KC_NO,     KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_VOLD,   KC_MUTE, KC_VOLU,          KC_NO,            KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
        ),
    [2] = LAYOUT(
        KC_NO,     KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO,
        KC_NO,     KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO,
        KC_NO,     KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO,
        KC_NO,     KC_NO,   KC_NO,            KC_NO,            KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
        ),
};
