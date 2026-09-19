# Hardware adapters

Hardware packages describe the board, buses, display, input and power. Shared
components/profiles provide the UI; device YAML owns the application tree and
installation bindings. Do not fork the UI to support another board.

Start with the [device adoption guide](../docs/DEVICE-ADOPTION.md).
The [display catalog](../docs/DISPLAY-CATALOG.md#tested-devices) is the central
register of evidence and remaining checks. A working driver or matching
resolution alone does not establish device support.

## Board notes and reusable compositions

### Guition JC3248W535CN

[Touch-panel notes](jc3248w535cn.md) cover wiring, LVGL-owned rotation,
initial USB/OTA evidence and recovery. The [board composer](jc3248w535cn.yaml)
is used by the [public bring-up firmware](../devices/jc3248w535cn.yaml).

### NodeMCU-32S + ST7735 + encoder

[Encoder-panel notes](nodemcu-32s-st7735.md) cover wiring, driver-owned 270-degree
rotation (160×128 logical pixels), deployment evidence and remaining checks.
Reuse the [board composer](nodemcu-32s-st7735.yaml) with the compact package.
The [MQTT example](../examples/mqtt-lights/compact.yaml) is a composition
fragment, not standalone firmware.

### Small monochrome displays and new candidates

[SSD1306 template](display/ssd1306-128x64.yaml) and
[tiny/readable host profiles](../profiles/README.md) provide starting points.
Physical OLED validation is still pending. Track additional targets in the
catalog; development branches and pull requests are not merged support.

## Capability folders

- display/: display-controller configuration.
- touchscreen/: touch controllers and their native coordinates.
- input/: encoder/button mappings.
- power/: backlight and power controls.
- bus/: board-specific SPI/QSPI/I2C wiring.
- Board composer YAMLs: assemble those capabilities without installation secrets.

Reuse compatible pieces after checking the exact board revision and pin map.
Choose rotation ownership once: LVGL for the JC3248W535CN, the display driver
for the ST7735 composition. Verify input coordinates as well as image orientation.

## Build and deployment

[Device compositions](../devices/README.md) distinguish complete firmware roots
from fragments and hardware packages. Follow the individual board notes for
build and recovery instructions; a USB port or flash layout from another board
must not be copied blindly. Keep backups, credentials and firmware outside Git.
