/*
Copyright 2021 projectcain
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE

#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP15
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 1000U

#define RP2040_FLASH_GENERIC_03H

// Enable RGB Matrix keypress reactive effects
#define RGB_MATRIX_KEYPRESSES

// Enable custom RGB Matrix effects
#define RGB_MATRIX_CUSTOM_KB

// Performance and stability optimizations
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS  // Enable more efficient frame buffering
#define RGB_MATRIX_DEFAULT_SPD 127      // Slower animation speed to reduce CPU load
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 200  // Reduce max brightness (was 255) to reduce power draw
#define RGB_MATRIX_LED_FLUSH_LIMIT 16   // Process fewer LEDs per iteration to avoid blocking
#define DEBOUNCE 5                      // Standard debounce time
#define USB_POLLING_INTERVAL_MS 1       // Fast USB polling
