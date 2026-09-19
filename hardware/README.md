# Hardware adapters

Hardware packages provide display, touch, backlight and platform configuration.
They compose the same application UI as the simulator; they do not duplicate
the navigation catalog or forms.

See the [display and interaction catalog](../docs/DISPLAY-CATALOG.md) for
reference formats and a tested-device register with evidence and open checks.

## Capability folders

Hardware is organized by capability. Board composers (e.g. `jc3248w535cn.yaml`)
import these pieces; device YAMLs import the board composer.

```
hardware/
├── display/          # Display controllers
│   ├── jc3248w535.yaml         # MIPI/QSPI 320×480 IPS LCD
│   ├── st7735-nodemcu-32s.yaml # SPI 160×128 TFT LCD
│   ├── st7735-160x128.yaml     # SPI TFT template (docs)
│   └── ssd1306-128x64.yaml     # I2C 128×64 OLED (docs/template)
├── touchscreen/      # Touch controllers
│   └── axs15231.yaml           # I2C capacitive touch
├── input/            # Physical input devices
│   └── encoder-nodemcu-32s.yaml # Rotary encoder + buttons
├── power/            # Power/backlight
│   ├── backlight-gpio1.yaml    # GPIO enable (JC3248W535CN)
│   └── backlight-gpio4-pwm.yaml # PWM LEDC (NodeMCU-32S)
├── bus/              # Communication buses
│   ├── jc3248w535cn-qspi.yaml  # QSPI for display
│   └── jc3248w535cn-i2c.yaml   # I2C for touch
├── jc3248w535cn.yaml # Board composer: touch panel
└── nodemcu-32s-st7735.yaml # Board composer: encoder panel
```

### Supported displays

| Display | Resolution | Interface | Profile | Status |
|---------|------------|-----------|---------|--------|
| JC3248W535 | 320×480 | MIPI/QSPI | regular | Verified |
| ST7735 | 160×128 | SPI | tft160 | Verified |
| SSD1306 | 128×64 | I2C | tiny/readable | Pending |

### Supported boards

| Board | Display | Input | Profile |
|-------|---------|-------|---------|
| JC3248W535CN | JC3248W535 | Touch | regular |
| NodeMCU-32S | ST7735 | Encoder | tft160 |

### Adding a new board

1. Create bus configs in `bus/` if the pins differ from existing configs.
2. Reference existing display/touchscreen/power packages, or add new ones.
3. Create a board composer YAML that imports the capabilities.
4. Create a device YAML in `devices/` that imports the board composer.

## Guition JC3248W535CN

Entry point: devices/jc3248w535cn.yaml. First physical bring-up passed on
2026-09-16: the owner confirmed a visible eight-icon launcher and working touch;
USB logs independently showed navigation through Settings and About.
The connected unit reports ESP32-S3 revision v0.2, 16 MiB flash and 8 MiB
embedded PSRAM. Board-specific pin mapping uses the JC3248W535 family model.

- Display: AXS15231B, native 320x480, QSPI clock GPIO47, data GPIO21/48/40/39.
- Chip select: GPIO45, supplied by the native ESPHome JC3248W535 model.
- Touch: AXS15231 over I2C, SDA GPIO4, SCL GPIO8, interrupt GPIO3.
- Backlight enable: GPIO1.
- Octal PSRAM at 80 MHz; ESP-IDF framework.
- USB Serial/JTAG logging; no Wi-Fi credentials, API or OTA in this fixture.

The board uses GPIO3/GPIO45 as documented signals despite their strapping roles;
do not attach additional pulls or change the wiring.

LVGL owns the initial 90-degree rotation. Keep the display and touch in native
coordinates; LVGL rotates pointer coordinates. nabla_initial_rotation initializes
both LVGL and the menu's rotation state. The desktop simulator defaults to zero.

The eight applications, forms, fonts, theme and animations are reused unchanged.
Wi-Fi remains the explicitly labeled M2 simulation. Clock displays --:-- until a
real time source is configured. This first image does not provision a network.

## Build

From the repository root in WSL:

    source .venv/bin/activate
    esphome compile devices/jc3248w535cn.yaml

ESPHome downloads the pinned ESP-IDF toolchain on the first build.
For this pinned ESP-IDF build, the factory image is:

    devices/.esphome/build/nabla-jc3248w535cn/build/firmware.factory.bin

The upload manifest is build/flasher_args.json in the same build tree. It contains
the bootloader at 0x0, partition table at 0x8000, initial OTA data at 0x9000 and
application at 0x10000. The factory image combines them; it is not an OTA image.

Copy the factory image to the Windows filesystem if the USB port belongs to
Windows. Confirm the actual port with python -m serial.tools.list_ports -v.
Close any serial monitor before uploading. On the first verified unit:

    python -m esptool --chip esp32s3 --port COM22 --baud 115200 --no-stub --after hard-reset write-flash --flash-mode dio --flash-freq 80m --flash-size 16MB 0x0 firmware.factory.bin

Run that command from the copied image's directory, or supply its absolute path.
It overwrites firmware: confirm the target and preserve its backup first.
The ROM path avoids the fast-stub transfer issue previously seen on this unit.
esptool 5.3.0 wrote the image and reported Hash of data verified, then reset the
board. No eFuses were changed and no whole-flash erase was required.

## Optional private Wi-Fi and OTA deployment

On 2026-09-16 the same board also passed a network-enabled private build:
USB writing was verified, Wi-Fi connected, and authenticated OTA completed.
A separate Home Assistant Device Builder then validated and compiled the private
composition using a pinned local library checkout, uploaded it successfully over
the routed network and reported the device online again. Its OTA transfer took
7.29 seconds. No private credentials or addresses are part of this repository.

See [private installations](../docs/PRIVATE-INSTALLATIONS.md) for composition.
This proves the static Wi-Fi/OTA deployment path, not runtime credential editing,
fallback/reconnect fault tests or completion of M3. The on-screen Wi-Fi editor
and header radio indicators remain the documented simulation/placeholders.

## Recovery

Before replacing existing firmware, retain a full flash backup outside Git.
Confirm the backup's size, hash and board identity against its private manifest.
The backup for the first unit was previously verified by device-side digest.
Do not publish it: it may include credentials and other device settings.

Restore only to the matching board, using its saved restoration instructions.
Do not alter eFuses, secure boot or flash encryption during UI bring-up.

## Evidence and limits

Chip/flash/PSRAM identification comes from esptool, not a vendor-memory assumption.
The visible launcher and touch navigation were confirmed on the connected unit.
The serial log showed Settings/About route changes following physical touches.
The first factory image was 760,960 bytes; the linker reported a 695,307-byte
application image and 65,119 bytes of static RAM. These are build figures, not
runtime heap high-water measurements.

All four runtime rotations, touch accuracy at every edge, physical text legibility,
prolonged animation, reconnect and the 24-hour soak remain unverified. This first
successful upload does not complete M3. See the roadmap for the remaining gates.

Sources for the initial hardware mapping:
- [ESPHome maintainer's JC3248W535 example](https://gist.github.com/clydebarrow/565251df8221f9045cd013dbc7faa3bf).
- Installed ESPHome 2026.8.2: components/mipi_spi/models/jc.py and axs15231.
- [Guition product specifications](https://www.guition.com/ku/icms/upload/fb081940d6fc11f09850077a33e1404f/FTPData/UEditor/file/2026121/1768961095054/JC3248W535%20Specifications-EN.pdf).

## NodeMCU-32S + ST7735 + encoder

See [the landscape encoder target](nodemcu-32s-st7735.md). The device owns its
GPIO/network/menu YAML and imports `packages/compact.yaml` at a pinned revision.
OTA installation, API reconnection and real MQTT state reception passed on
2026-09-18. Physical color/encoder usability and soak validation remain pending;
see the target notes for exact evidence and build resource figures.

## T-Call + SSD1309 OLED + encoder

The [128×64 monochrome assembly](tcall-ssd1309.md) now has a
[public hardware composer](tcall-ssd1309.yaml) and
[complete device root](../devices/tcall-ssd1309.yaml).
Physical operation was confirmed by the owner after OTA on 2026-09-19;
network recovery and long-duration qualification remain separate checks.
