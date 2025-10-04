// Copyright 2024 Georg (@technofrikus)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/* SPI config for display */
#define SPI_DRIVER SPID1
#define SPI_SCK_PIN GP10
#define SPI_MOSI_PIN GP11
#define SPI_MISO_PIN NO_PIN //not necessary

/* LCD config */
#define LCD_DC_PIN GP12
#define LCD_CS_PIN GP9
#define LCD_RST_PIN GP13
#define BACKLIGHT_PWM_DRIVER PWMD7 //is used for the backlight of the LCD, PIN is configured in info.json
#define BACKLIGHT_PWM_CHANNEL RP2040_PWM_CHANNEL_A
#define BACKLIGHT_LEVELS 31
#define QUANTUM_PAINTER_DISPLAY_TIMEOUT 0 //timeout of shutting the display off after last user input
#define QUANTUM_PAINTER_SUPPORTS_NATIVE_COLORS TRUE
