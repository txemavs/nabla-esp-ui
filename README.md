# ∇ nabla-esp-ui

A modular UI platform for **ESPHome + LVGL**: define applications and navigation
in YAML, reuse presentation and interaction across panels.

Works toward standalone devices, optional Home Assistant integration, Nabla Edge
and cooperation between large and small screens. Local operation must not depend
on a server being available.

**Status:** working desktop prototype on ESPHome 2026.8.2 / LVGL 9.5.
A JC3248W535CN now runs the launcher with owner-confirmed touch navigation.
Persistent settings, radio integration and device-to-device links remain unvalidated.
Start with [the simulator](simulator/README.md).

## Platform plan

The [platform design](docs/platform/README.md) describes the architecture,
component catalog, readable/tiny display profiles, keyboards and forms, Wi-Fi
commissioning, peer control, and the vehicle panel with a Raspberry Pi 4.
The [roadmap](docs/platform/ROADMAP.md) defines implementation order and exit gates.

M1 adaptive profiles and M2 local forms are implemented on the host. Try the
Wi-Fi simulation or Settings > Controls (demo); real radios are the next milestone.
Draft YAML in docs/platform/ is explicitly illustrative, not accepted firmware.

## What works today

- One declarative navigation tree with up to eight children per node.
- Eight-tile desktop or scrollable list, switched with the root triangle.
- Nested navigation with clickable ancestors, restored focus and scroll.
- Touch and keyboard; Up/Down plus Enter emulate rotary navigation.
- A root rotation button cycles 90 degrees; grid reflows 4x2 or 2x4.
- Shared compact header/footer, configurable logo and status icon assets.
- Simulated startup progress followed by brand and HH:MM desktop footer.
- Dark/light appearance and build-time Spanish/English with accent glyphs.
- Native 128x64 tiny/readable and 320x480 portrait profiles.
- Shared simulated Wi-Fi scans, masked password editing and failure/retry states.
- YAML-defined numeric, choice and toggle fields with cancel/confirm transactions.

The current desktop contains Settings, Communications, Photos, Music, Cameras,
Weather, Lights and Sensors. Most are placeholders; Settings demonstrates
appearance and nested navigation, not completed network configuration.
Wi-Fi/Bluetooth header icons are visual placeholders, not live connection status.

## How the repository fits together

- theme/: colors, typography, glyphs and assets.
- components/: reusable visual pieces and [catalog](components/README.md).
- navigation/: interaction, rendering and [tree contract](navigation/README.md).
- external_components/nabla_navigation/: validation and generated C++ descriptors.
- locales/: build-time translations.
- examples/hello-world/: current panel composition.
- simulator/hardware/: SDL, keyboard, mouse and host clock adapters.
- docs/platform/: proposed modules, profiles, integrations and release plan.

Native ESPHome packages and includes assemble firmware. The local external
component validates the navigation YAML during normal ESPHome compilation.
There is no mandatory standalone generation step.

## Design principles

Application meaning is separate from placement and transport. A light control
should bind to a local entity, Home Assistant or Edge without reimplementing its
UI. The same menu becomes tiles on a large touch screen and a readable list on
a smaller device; it is not just scaled down.

Every interaction must have a path using UP/DOWN/ENTER and accessible Back.
Use typed state for unknown/stale/offline values and bounded asynchronous work.
Add a new capability as a documented module with a runnable example.
A stable core still receives maintenance; it is not a promise of zero future edits.

The first physical target is **JC3248W535CN**: USB installation, visible launcher
and touch navigation have passed an initial check. See [the hardware adapter](hardware/README.md).
The 128x64 host profiles are implemented; physical OLED and input adapters remain planned.
Host rendering alone does not establish hardware support; the initial board check
is narrower than complete M3 validation.

## Brand and contributions

Brand: nabla.net ESP UI. Preserve the original triangle artwork in assets/nabla.jpg.
See [brand identity](docs/BRAND.md) and [contributor instructions](AGENTS.md).
Documentation and identifiers use English; UI strings are localized.
Keep bundled font licenses and all applicable third-party attribution.

## References

- [ESPHome LVGL](https://esphome.io/components/lvgl/)
- [ESPHome packages](https://esphome.io/components/packages/)
- [SDL host display](https://esphome.io/components/display/sdl/)
- [ESPHome UI Kit](https://github.com/mplogas/esphome-ui-kit)
- [ESPHome Modular LVGL Buttons](https://github.com/agillis/esphome-modular-lvgl-buttons)

External projects are references, not claims of API compatibility.
See [research notes](docs/platform/SOURCES.md) for the platform plan.
