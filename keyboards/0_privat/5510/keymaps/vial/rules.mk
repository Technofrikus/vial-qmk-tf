VIA_ENABLE = yes
VIAL_ENABLE = yes
ENCODER_ENABLE = yes
ENCODER_MAP_ENABLE = yes
VIALRGB_ENABLE = yes
QUANTUM_PAINTER_ENABLE = yes
QUANTUM_PAINTER_DRIVERS += ili9488_spi

SRC += fonts/SFProDisplayLight16.qff.c
SRC += fonts/SFProDisplayLight20.qff.c
SRC += fonts/SFProDisplaySemibold16.qff.c
SRC += fonts/SFProDisplaySemibold20.qff.c

SRC+= graphics/ClackyRainbow_480x320.qgf.c
