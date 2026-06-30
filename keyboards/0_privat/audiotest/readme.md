# audiotest

Minimal RP2040 QMK/Vial board for testing a PAM8403 amplifier and speaker with
a playful "yapyap" speech-imitation synth.

## Hardware

- MCU: RP2040
- Bootloader: rp2040 (UF2, double-tap reset)
- Amplifier: PAM8403 input on `GP27` (PWM5 channel B) → speaker
- Key: switch on `GP29` (direct pin, active-low to GND) - default `KC_A`

## Behavior

- On power-up: plays a short startup chord (E6, C6, G6) through the amplifier.
- Key (`KC_A`, `GP29`): plays ~10 seconds of random yapyap syllables through the
  amplifier (80 notes, 16th-note timing × 4 ≈ 125 ms per note).

Default keycode can be remapped in Vial.

## Build

```
make 0_privat/audiotest:vial
```

## Flash

Hold the BOOTSEL button while plugging in (or double-tap reset) to enter the
RP2040 mass-storage bootloader, then:

```
make 0_privat/audiotest:vial:flash
```
