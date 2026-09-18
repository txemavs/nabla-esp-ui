# ∇ nabla-esp-ui

A modular UI platform for **ESPHome**, with LVGL and compact display renderers: define applications and navigation
in YAML, reuse presentation and interaction across panels.

Works toward standalone devices, optional Home Assistant integration, Nabla Edge
and cooperation between large and small screens. Local operation must not depend
on a server being available.

**Status:** working desktop prototype on ESPHome 2026.8.2 / LVGL 9.5.
A JC3248W535CN now runs the launcher with owner-confirmed touch navigation.
An optional primary Wi-Fi adapter has passed initial physical scan/save/rollback
checks; peer links and full hardware recovery/soak coverage remain pending.
A [NodeMCU-32S/ST7735 encoder panel](hardware/nodemcu-32s-st7735.md) also runs
the 160x128 compact UI, live information and MQTT control; OTA/API and MQTT
reception are verified. Physical usability and soak gates remain open.
Start with [the simulator](simulator/README.md).

## Use from GitHub

Start with [the editable device YAML](examples/github/panel.yaml) and the
[GitHub library guide](docs/GITHUB-LIBRARY.md). The library is fetched at build
time; your navigation, bindings and secrets remain in your own configuration.

## Optional camera service

[Home Assistant camera cache](services/homeassistant/README.md) provides protected,
on-demand 64x64 thumbnails and JPEGs up to 480 pixels, shared by all viewers.

## Platform plan

The [platform design](docs/platform/README.md) describes the architecture,
component catalog, readable/tiny display profiles, keyboards and forms, Wi-Fi
commissioning, peer control, and the vehicle panel with a Raspberry Pi 4.
The [roadmap](docs/platform/ROADMAP.md) defines implementation order and exit gates.

M1 adaptive profiles and M2 local forms are implemented on the host. Try the
Wi-Fi simulation or Settings > Controls (demo). The private physical composition
can opt into real Wi-Fi with primary/fallback recovery.
Draft YAML in docs/platform/ is explicitly illustrative, not accepted firmware.

## What works today

- One declarative navigation tree with up to eight children per node.
- Eight-tile desktop or scrollable list, switched with the root triangle.
- Nested navigation with clickable ancestors, restored focus and scroll.
- Touch and keyboard; Up/Down plus Enter emulate rotary navigation.
- A root rotation button cycles 90 degrees; grid reflows 4x2 or 2x4.
- Shared compact header/footer, configurable logo and status icon assets.
- Immediate launcher startup, reusable operation progress and brand/HH:MM footer.
- Settings > Information shows model, network, ESPHome version and uptime.
- Dark/light appearance and build-time Spanish/English with accent glyphs.
- Native 128x64 tiny/readable, 160x128 color/encoder and 320x480 portrait profiles.
- Optional compact MQTT light control and a pinned GitHub captive portal.
- Shared simulated Wi-Fi scans, masked password editing and failure/retry states.
- YAML-defined numeric, choice and toggle fields with cancel/confirm transactions.

The current desktop contains Settings, Communications, Photos, Music, Cameras,
Weather, Lights and Sensors. Most are placeholders; Settings demonstrates
appearance and nested navigation, not completed network configuration.
Wi-Fi/Bluetooth header icons are visual placeholders, not live connection status.
An opt-in [primary Wi-Fi adapter](external_components/nabla_wifi/README.md) connects
the LVGL form to real scans and a saved primary without losing configured fallbacks.

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

## Nabla Net MQTT

Panels can control Home Assistant lights across multiple sites through a shared
MQTT broker, without native API connections. See [the protocol](docs/NABLA-NET-MQTT.md)
for topic structure, freshness rules and broker configuration. The
[mqtt-lights adapter](adapters/mqtt-lights/README.md) implements the protocol;
[examples/mqtt-lights](examples/mqtt-lights/) shows synthetic device compositions.

## Private installations

Keep site-specific device YAML, camera/entity bindings and secrets outside this
public repository. See [the public/private boundary](docs/PRIVATE-INSTALLATIONS.md)
for ownership, routed OTA and a Device Builder composition using a pinned local
library checkout. The [private device checklist](docs/PRIVATE-DEVICE-TEMPLATE.md)
covers multi-network Wi-Fi, stable OTA addresses and MQTT client_id conventions.

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
