# Moinboards Kastenwagen Alu

_PCB for the Kastenwagen Alu_

-   Keyboard Maintainer: [Technofrikus](https://github.com/Technofrikus)
-   Onboard STM32 MCU
-   Based on the original PCB by NoPunin10did, but basically completeley new and NOT compatible with the previous (printed) Kastenwagen case or PCB.
-   Works with VIAL

Make example for this keyboard (after setting up your build environment):

    make moinboards/kastenwagen_alu:vial

Flashing example for this keyboard:

    make moinboards/kastenwagen_alu:vial:flash

-   **Bootmagic reset**: Hold down the key at (0,0) in the matrix (usually the top left key or Escape) and plug in the keyboard
-   **Physical reset button**: Briefly press the button on the back of the PCB - some may have pads you must short instead
-   **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
