// Copyright 2026 technofrikus
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include <stdlib.h>

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(KC_A),
};

#ifdef AUDIO_ENABLE
#include "audio.h"

// Higher-pitched syllables for the amplified speaker (~125 ms per note)
static float yap_ya[2][2] = SONG(S__NOTE(_E5), S__NOTE(_A6));
static float yap_ba[2][2] = SONG(S__NOTE(_C5), S__NOTE(_C6));
static float yap_do[2][2] = SONG(S__NOTE(_G6), S__NOTE(_D5));
static float yap_ko[2][2] = SONG(S__NOTE(_F5), S__NOTE(_F4));
static float yap_li[2][2] = SONG(S__NOTE(_E6), S__NOTE(_B6));
static float yap_mu[2][2] = SONG(S__NOTE(_C5), S__NOTE(_F4));
static float yap_pe[2][2] = SONG(S__NOTE(_F5), S__NOTE(_E6));
static float yap_wa[2][2] = SONG(S__NOTE(_A4), S__NOTE(_A6));

static float (*const syllables[])[2] = {
    yap_ya, yap_ba, yap_do, yap_ko, yap_li, yap_mu, yap_pe, yap_wa,
};
#define NUM_SYLLABLES (sizeof(syllables) / sizeof(syllables[0]))

#define YAPYAP_NOTE_COUNT 80
#define YAPYAP_DURATION_MULT 4U

static float yapyap_song[YAPYAP_NOTE_COUNT][2];

static void play_10s_yapyap(void) {
    for (uint8_t i = 0; i < YAPYAP_NOTE_COUNT; i += 2) {
        uint8_t idx = rand() % NUM_SYLLABLES;
        for (uint8_t n = 0; n < 2; n++) {
            yapyap_song[i + n][0] = syllables[idx][n][0];
            yapyap_song[i + n][1] = syllables[idx][n][1] * YAPYAP_DURATION_MULT;
        }
    }
    audio_play_melody(&yapyap_song, YAPYAP_NOTE_COUNT, false);
}

void keyboard_post_init_user(void) {
    srand(timer_read32());
    static float startup_song[][2] = SONG(Q__NOTE(_E6), Q__NOTE(_C6), Q__NOTE(_G6));
    PLAY_SONG(startup_song);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return true;
    }

    if (keycode == KC_A) {
        play_10s_yapyap();
    }
    return true;
}
#endif
