// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "display.c"

#include "print.h" //for debug


// Keymap
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT_macro(
        QK_BOOT,   KC_B,   KC_C,   KC_D,
        KC_E,   KC_F,   KC_G,   KC_H,
        KC_L,   KC_K,   KC_J,   KC_I
    )
};

//Encoder
#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] =   { ENCODER_CCW_CW(LSA(KC_VOLD), LSA(KC_VOLU))  },

};
#endif








/* void housekeeping_task_user(void) {
    static uint32_t last_draw = 0;
    //uint16_t keycode_dyn;
    if (timer_elapsed32(last_draw) > 33) { // Throttle to 30fps
        last_draw = timer_read32();

        }

    }; */
//};
/*
uint8_t current_layer = 0;
layer_state_t layer_state_set_user(layer_state_t state) {
    current_layer = get_highest_layer(state);
    return state;

static painter_image_handle_t myimage; //Image Object
*/

static const char* PROGMEM code_to_name[256] = {
//   0    1    2    3    4    5    6    7    8    9    A    B    c    D    E    F
    " ", " ", " ", " ", "aaa", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l",  // 0x
    "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2",  // 1x
    "3", "4", "5", "6", "7", "8", "9", "0",  "20",  "19",  "27",  "26",  "22", "-", "=", "[",  // 2x
    "]","\\", "#", ";","\"", "`", ",", ".", "/", " "," "," "," "," "," "," ",  // 3x
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",  // 4x
    " "," "," ", "N", "/", "*", "-", "+",  "23", "1", "2", "3", "4", "5", "6", "7",  // 5x
    "8", "9", "0", ".","\\", "A",   "0", "=", " ", " ", " ", " ", " ", " ", " ", " ",  // 6x
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",  // 7x
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",  // 8x
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",  // 9x
    " ", " ", " ", " ", " ",   "0", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",  // Ax
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",  // Bx
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",  // Cx
    " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",  // Dx
    "C", "Shift", "A", "C", " ", " ", " ", " ", " ", " ", " ", " ", " ",  " "," "," ",  // Ex
     " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",   // Fx
};

const char* xxx(uint16_t keycode, keyrecord_t* record) {
    if (keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) {
        keycode = QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
    } else if (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX) {
        keycode = QK_LAYER_TAP_GET_TAP_KEYCODE(keycode);
    } else if (keycode >= QK_MODS && keycode <= QK_MODS_MAX) {
        keycode = QK_MODS_GET_BASIC_KEYCODE(keycode);
    }

    if (keycode < ARRAY_SIZE(code_to_name)) {
        return code_to_name[keycode];
    } else {return "gibn" ;}
}


 bool process_record_user(uint16_t keycode, keyrecord_t *record) {
      // If console is enabled, it will print the matrix position and status of each key pressed
    #ifdef CONSOLE_ENABLE
    uint8_t col = record->event.key.col;
    uint8_t row = record->event.key.row;
    uint16_t keycode_dyn = dynamic_keymap_get_keycode(current_layer,row,col);
    const char* desc = xxx(keycode_dyn, record);
    bool isShift = (keycode_dyn & MOD_MASK_SHIFT) > 0;  // funktioniert noch nicht, keycode_dyn Ausgabe prüfen?
    bool isCtrl = (keycode_dyn & MOD_MASK_CTRL) > 0;
    bool isAlt = (keycode_dyn & MOD_MASK_ALT) > 0;

    // Nächstes: Zwei Strings konkatinieren, wie geht das in QMK?

    uprintf("Layer: %u, kc: %u / 0x%04X, col: %2u, row: %2u, desc: %s, shift: %d, ctrl: %d, alt: %d\n", current_layer, keycode, keycode_dyn, col, row, desc, isShift, isCtrl, isAlt);
    #endif
    return true;
    };

/*

    myimage = qp_load_image_mem(gfx_clackyrainbow120);
    uprintf("image loaded \n");
    //if (myimage != NULL) {
    qp_drawimage(display, 0, 0, myimage);
    uprintf("image drawn \n");
      return true;




    Notiz:
    keycode_dyn = dynamic_keymap_get_keycode(0,0,1);
    printf("keycode: %u", keycode_dyn);
    print("Housekeeping\n");







     */

