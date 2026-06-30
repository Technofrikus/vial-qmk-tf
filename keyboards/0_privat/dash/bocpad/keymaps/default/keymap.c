// Copyright 2026 Technofrikus
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_2,    KC_1,    KC_6,
        KC_3,    KC_4,    KC_5,
        KC_7
    ),
};
