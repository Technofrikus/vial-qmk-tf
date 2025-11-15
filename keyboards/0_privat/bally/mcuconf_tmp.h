#include_next "mcuconf.h"

/* Trackball */

#ifdef POINTING_DEVICE_ENABLE
#    undef RP_SPI_USE_SPI0
#    define RP_SPI_USE_SPI0 FALSE
#    undef RP_SPI_USE_SPI1
#    define RP_SPI_USE_SPI1 TRUE
#    define SPI_DRIVER SPID1
#    define SPI_SCK_PIN GP2
#    define SPI_MISO_PIN GP4
#    define SPI_MOSI_PIN GP3
#endif

