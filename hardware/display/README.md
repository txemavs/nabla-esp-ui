# Display drivers

ESPHome display component configurations for supported panels.

## Available displays

| File | Controller | Resolution | Interface | Notes |
|------|------------|------------|-----------|-------|
| jc3248w535.yaml | JC3248W535 | 320×480 | MIPI/QSPI | IPS LCD, verified |
| st7735-nodemcu-32s.yaml | ST7735 | 160×128 | SPI | TFT LCD, verified |
| st7735-160x128.yaml | ST7735 | 160×128 | SPI | TFT LCD, template |
| ssd1306-128x64.yaml | SSD1306 | 128×64 | I2C | OLED, template |

## Usage

Import via the board composer in `hardware/`:

```yaml
packages:
  display: !include display/jc3248w535.yaml
```

Or reference from a device YAML through the board composer.

## Dependencies

Each display may require:
- A bus configuration from `bus/` (SPI, I2C, QSPI)
- A backlight/power configuration from `power/`
- Specific GPIO pins defined by the board

## Template files

Files marked as templates (st7735, ssd1306) document the display type and
provide example configurations. Board-specific pin assignments vary by wiring
and are owned by the device YAML or board composer.
