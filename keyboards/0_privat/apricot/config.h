// Copyright 2023 LazyDesigners (@LazyDesigners)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */


/* disable action features */
#define NO_ACTION_ONESHOT


//safe some space
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE

// combos always reference to QWERTY
#define COMBO_ONLY_FROM_LAYER 0

#define WS2812_DI_PIN B3
