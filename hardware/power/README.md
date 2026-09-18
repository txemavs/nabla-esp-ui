# Power and backlight

ESPHome power supply and backlight configurations.

## Available configurations

| File | Function | Notes |
|------|----------|-------|
| backlight-gpio1.yaml | Display backlight enable | GPIO1, enable on boot |

## Usage

Import via the board composer:

```yaml
packages:
  backlight: !include power/backlight-gpio1.yaml
```

## Adding new configurations

For boards with different backlight pins or PWM brightness control,
add a new YAML file here. Name it descriptively (e.g. `backlight-gpio4-pwm.yaml`).
