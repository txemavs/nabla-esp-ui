# Touchscreen drivers

ESPHome touchscreen component configurations.

## Available touchscreens

| File | Controller | Interface | Notes |
|------|------------|-----------|-------|
| axs15231.yaml | AXS15231 | I2C | Capacitive, used with JC3248W535CN |

## Usage

Import via the board composer:

```yaml
packages:
  touch: !include touchscreen/axs15231.yaml
```

## Dependencies

- I2C bus configuration from `bus/`
- Display reference (`panel_display`)
- Interrupt GPIO pin (board-specific)
