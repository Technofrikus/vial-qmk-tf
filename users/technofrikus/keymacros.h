#pragma once
#include "technofrikus.h"
#include "keymap_german_mac_iso.h"

#define LAYOUT_wrapper(...) LAYOUT(__VA_ARGS__)


#define VOLU LSA(KC_KB_VOLUME_UP)
#define VOLD LSA(KC_KB_VOLUME_DOWN)

#define ___________________________________________ _______, _______, _______, _______, _______

#define _________________QWERTY_L1_5_______________ KC_Q,    KC_W,    KC_E,    KC_R,    KC_T
#define _________________QWERTY_L2_5_______________ KC_A,    KC_S,    KC_D,    KC_F,    KC_G
#define _________________QWERTY_L3_5_______________ KC_Z,    KC_X,    KC_C,    KC_V,    KC_B

#define _________________QWERTY_R1_5_______________ KC_Y,    KC_U,    KC_I,    KC_O, LCAG_T(KC_P)
#define _________________QWERTY_R2_4_______         TD(0),    KC_J,    KC_K,    KC_L
#define _________QWERTY_R3_3_______                 KC_N,    KC_M,    KC_COMM

#define _________________QAZ_L1_5_______________ KC_Q,    KC_W,    KC_E,    KC_R,    KC_T
#define _________________QAZ_L2_5_______________ LCTL_T(KC_A),    KC_S,    KC_D,    KC_F,    KC_G
#define _________________QAZ_L3_5_______________ LSFT_T(KC_Z),    KC_X,    KC_C,    KC_V,    KC_B

#define ___________________SYM_L1_5________________ DE_DEG, DE_CIRC, DE_LCBR, DE_RCBR, DE_DLR
#define ___________________SYM_L2_5________________ DE_ADIA, DE_SS, DE_HASH, DE_EXLM, DE_MINS
#define ___________________SYM_L3_5________________ DE_LABK, DE_RABK, DE_LBRC, DE_RBRC, DE_ACUT

#define ___________________SYM_R1_5________________ DE_DQUO, DE_UDIA, DE_AMPR, DE_ODIA, DE_SECT
#define ___________________SYM_R2_4_________        DE_PERC, DE_QUES, DE_LPRN, DE_RPRN
#define ___________________SYM_R3_3_________        DE_GRV,  DE_UNDS, DE_PIPE

#define ___________________NUM_L1_5________________ KC_1, KC_2, KC_3, KC_4, KC_5
#define ___________________NUM_L2_5________________ _______, _______, _______, _______, _______
#define ___________________NUM_L3_5________________ _______, _______, _______, _______, _______

#define ___________________NUM_R1_5________________ KC_6,    KC_7,    KC_8,    KC_9,    KC_0
#define ___________________NUM_R2_5________________ DE_PLUS, DE_MINS, DE_ASTR, DE_SLSH, DE_COMM
#define ___________________NUM_R3_3_________        _______, DE_EQL,  DE_DOT

#define ___________________NAV_L1_Mouse_5__________ _______, _______, KC_MS_WH_UP, _______, _______
#define ___________________NAV_L2_Mouse_5__________ _______,KC_MS_WH_LEFT, KC_MS_WH_DOWN, KC_MS_WH_LEFT, _______
#define ___________________NAV_L3_Mouse_5__________ _______, QK_BOOT, _______, _______, _______

#define ___________________NAV_L1_5________________ _______,  _______, _______, _______, _______
#define ___________________NAV_L2_5________________ _______, _______, _______, _______, _______
#define ___________________NAV_L3_5________________ _______, QK_BOOT, _______, _______, _______

#define ___________________NAV_R1_5________________ _______, LALT(KC_LEFT), KC_UP, LALT(KC_RIGHT), KC_END
#define ___________________NAV_R2_5________________ KC_HOME, KC_LEFT, KC_DOWN, KC_RIGHT,  _______
#define ___________________NAV_R3_5________________ _______, _______, _______, _______, _______

#define __________________GAME_L1_5________________ KC_Q,    KC_W,    KC_E,    KC_R,    KC_T
#define __________________GAME_L2_5________________ KC_A,    KC_S,    KC_D,    KC_F,    KC_G
#define __________________GAME_L3_5________________ KC_Z,    KC_X,    KC_C,    KC_V,    KC_B

#define __________________GAME_R1_5________________ KC_Y,    KC_U,    KC_I,    KC_O,    KC_P
#define __________________GAME_R2_5________________ KC_H,    KC_J,    KC_K,    KC_L,    KC_ENT
#define ____________GAME_R3S______________          KC_N,    KC_M,    KC_COMM, KC_DOT
#define __________________GAME_R3__________________ ____________GAME_R3S______________, _______

#define __________SPC_3__________ MO(_SYM), KC_ENT, LT(_NUM, KC_SPC)
#define _________GAME_SPC_3______ KC_SPC, KC_ENT,  KC_SPC

#define ______SPC_2_____ MO(_SYM), LT(_NUM, KC_SPC)
#define ___GAME_SPC_2___ KC_SPC,  KC_SPC

#define __________LMODS_3_________ KC_LCTL, KC_LALT, KC_LGUI
