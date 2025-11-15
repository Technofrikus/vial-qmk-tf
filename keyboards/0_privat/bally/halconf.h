#include_next "halconf.h"

/* Trackball */
#ifdef POINTING_DEVICE_ENABLE
#    undef HAL_USE_SPI
#    define HAL_USE_SPI TRUE
#endif
