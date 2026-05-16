# Disarray Desnarler

![Disarray Desnarler](https://raw.githubusercontent.com/ZenVega/disarray_desnarler_docs/main/images/desnarler.jpg)

A 4-key RP2040 macropad with an analog slider, a two-position switch, and three indicator LEDs.

- Keyboard Maintainer: [42fh](https://github.com/42fh)
- Hardware Supported: Disarray Desnarler v1 PCB with Seeed Studio XIAO RP2040
- Hardware Availability: [Disarray Desnarler documentation](https://github.com/ZenVega/disarray_desnarler_docs), [Hackaday project](https://hackaday.io/project/204536-disarray-desnarler)

Make example for this keyboard (after setting up your build environment):

```sh
qmk compile -kb zenvega/disarray_desnarler -km default
```

Flashing example for this keyboard:

```sh
qmk flash -kb zenvega/disarray_desnarler -km default
```

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with the [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 3 ways:

- **Bootmagic reset**: Hold the top-left key while plugging in the keyboard.
- **Physical BOOT button**: Hold the BOOT button on the XIAO RP2040 while plugging in the keyboard.
- **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available.
