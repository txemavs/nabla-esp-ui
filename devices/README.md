# Device compositions

Device YAMLs are build entry points that compose hardware, UI profile, locale
and menu packages. They do not duplicate hardware definitions.

## Structure

```
devices/
└── jc3248w535cn.yaml    # Guition touch panel
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

## Entry points and other compositions

This directory currently contains the complete public touch-panel bring-up root.
It is not the full supported-hardware inventory. The ST7735 encoder target has
[a reusable board composer](../hardware/nodemcu-32s-st7735.yaml) and
[board notes](../hardware/nodemcu-32s-st7735.md); its deployed installation root
stays private. The compact MQTT example is an illustrative fragment.
Host roots live under [simulator/](../simulator/README.md).

See the [catalog](../docs/DISPLAY-CATALOG.md#tested-devices) for evidence and
[adoption guide](../docs/DEVICE-ADOPTION.md) for adding a complete target.
The composition sketch above illustrates the public bring-up fixture; deployed
consumers import the library entry point and declare their own navigation tree.

## Complete public device roots

| Device | Board | Display | Profile |
|--------|-------|---------|---------|
| jc3248w535cn.yaml | Guition JC3248W535CN | 320×480 IPS | regular |

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

See [JC3248W535CN notes](../hardware/jc3248w535cn.md) for flashing instructions.
