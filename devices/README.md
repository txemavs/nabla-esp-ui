# Device compositions

Device YAMLs are build entry points that compose hardware, UI profile, locale
and menu packages. They do not duplicate hardware definitions.

## Structure

```
devices/
├── jc3248w535cn.yaml       # Guition touch panel (320×480)
└── lilygo-twatch-2020.yaml # LilyGO T-Watch 2020 (240×240)
```

## Composition pattern

A device YAML imports packages rather than defining hardware inline:

```yaml
# Device-specific settings only
substitutions:
  ui_language: es
  nabla_initial_rotation: "90"
  screen_width: "480"
  screen_height: "320"

esphome:
  name: my-device
  friendly_name: My Device

packages:
  hardware: !include ../hardware/jc3248w535cn.yaml   # Board + display + touch
  locale: !include ../locales/${ui_language}.yaml    # es/en strings
  theme: !include ../theme/nabla.yaml                # Colors and fonts
  profile: !include ../profiles/regular.yaml         # 480×320 LVGL shell
  ui: !include ../examples/hello-world/ui.yaml       # Navigation menu
```

The hardware package (e.g. `hardware/jc3248w535cn.yaml`) is itself a composer
that imports capability packages from `hardware/display/`, `hardware/bus/`, etc.

## Available devices

| Device | Board | Display | Profile |
|--------|-------|---------|---------|
| jc3248w535cn.yaml | Guition JC3248W535CN | 320×480 IPS | regular |
| lilygo-twatch-2020.yaml | LilyGO T-Watch 2020 | 240×240 TFT | tft240 |

### LilyGO T-Watch 2020

The T-Watch 2020 is an ESP32-based smartwatch with 240×240 ST7789 display,
FT6336 capacitive touch, and AXP202 PMU. **PSRAM is required** (quad mode 80MHz)
for the display framebuffer—without it the display stays black.

The example uses a 2×2 tile launcher optimized for the square screen and
Kit1-style navigation (Settings, Control with generic Site A lights).

See [hardware/README.md](../hardware/README.md) for pin mapping and V1/V2/V3
differences.

## Private devices

Installation-specific device YAMLs with credentials, network configuration and
custom menus belong outside this repository. See [private installations](../docs/PRIVATE-INSTALLATIONS.md).

Import packages from the public library at a pinned SHA:

```yaml
packages:
  library:
    url: https://github.com/txemavs/nabla-esp-ui
    ref: ${nabla_ui_ref}
    refresh: never
    files:
      - packages/regular.yaml
```

## Build

From the repository root:

```bash
source .venv/bin/activate
esphome compile devices/jc3248w535cn.yaml
```

See [hardware/README.md](../hardware/README.md) for flashing instructions.
