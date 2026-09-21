# ∇ nabla-esp-ui

**One UI library. Touch screens, rotary encoders and small displays.**

[Getting started](docs/GETTING-STARTED.md) · [Display and test catalog](docs/DISPLAY-CATALOG.md) · [Documentation map](docs/README.md)

Build ESPHome panels with menus defined in YAML, reusable controls and a
consistent visual style. Use LVGL on larger screens or the compact display
renderer on smaller devices. Home Assistant and MQTT are optional integrations;
local navigation and settings do not depend on a server.

![Nabla ESP UI running on a large touch panel and a small color display with a rotary encoder](docs/images/nabla-panels-touch-and-encoder.png)

*Two real devices running Nabla ESP UI: JC3248W535CN touch panel on the left,
NodeMCU-32S with a 160×128 ST7735 display and encoder on the right.
The menus shown belong to a private installation; the reusable library is public.*

### Monochrome panel in a reused telephone

<img src="docs/images/tcall-ssd1309-phone.jpg" alt="T-Call with a 128×64 OLED and rotary encoder in a desk-phone enclosure" width="420">

*The owner-confirmed T-Call/SSD1309 assembly running the compact interface.
The photograph shows a private menu; public examples use generic bindings.
See [device YAML, wiring and verification](hardware/tcall-ssd1309.md).*

## Shared UI contract

**Declare what the device does; Nabla owns how its standard UI looks and works.**
Device YAML supplies menus, content and actions. Shared components and profiles
own typography, spacing, borders, focus and supported presentation. System apps
and Nabla widgets embedded in custom applications obey the same global settings.
Custom drawing stays inside its application surface and preserves a usable exit.

Existing themes, fonts, border modes and view choices are the starting point.
Consistent global icon visibility and coordinated size choices are requirements
to verify and complete across components, not a claim that every combination
already works. See [the UI contract and remaining work](docs/UI-CONSISTENCY.md).

## Start here

- **Use the library:** [library contract](docs/LIBRARY-CONTRACT-v0.1.md),
  [GitHub setup guide](docs/GITHUB-LIBRARY.md) and
  [editable device YAML](examples/github/panel.yaml).
- **Try it on your computer:** [simulator guide](simulator/README.md).
- **Choose hardware:** [touch panel](hardware/jc3248w535cn.md) or
  [ST7735 encoder panel](hardware/nodemcu-32s-st7735.md).
- **Adapt another device:** [adoption workflow and evidence](docs/DEVICE-ADOPTION.md),
  covering touch panels, compact encoders and monochrome displays.
- **Follow development:** [platform plan](docs/platform/README.md) and
  [milestones](docs/platform/ROADMAP.md).

Import packages/regular.yaml for LVGL or packages/compact.yaml for the compact
renderer. Pin a tested Git commit. Your device YAML owns its menus, hardware,
network configuration and bindings; GitHub supplies the reusable components,
fonts and assets. Nothing needs to be downloaded by the device at startup.

## What you can build

- Nested menus with tiles, lists, scrolling, focus and reachable Back controls.
- Large touch layouts, compact four-icon launchers and readable single-item views.
- Touch, keyboard and a deployed GPIO encoder/push/K0 composition.
- Dark/light themes, Ubuntu Mono or DejaVu Sans, color icons and borderless focus.
- Live device information: model, Wi-Fi, IP, router, signal, version and uptime.
- Light controls through optional [Home Assistant](adapters/ha-lights/README.md)
  and [MQTT adapters](adapters/mqtt-lights/README.md), with explicit availability
  and a confirmation step for compact brightness changes.
- Camera previews backed by an optional [Home Assistant image cache](services/homeassistant/README.md):
  shared, on-demand 64×64 thumbnails and JPEGs up to 480 pixels.
- Wi-Fi provisioning through an optional [primary Wi-Fi adapter](external_components/nabla_wifi/README.md)
  or [captive portal](external_components/captive_portal/README.md) fetched from GitHub.
- Shared text/password, numeric, choice and toggle form prototypes with
  cancel/confirm behavior.

The bundled desktop is an example catalog, not a complete application suite.
Its Wi-Fi forms and some status indicators are simulations unless an actual
adapter is configured. Compact Wi-Fi is also available on the 160×128 ST7735 through the shared
real Wi-Fi adapter and encoder password editor. The captive portal remains an
optional recovery/provisioning path.

## Current status

**Active development, pre-1.0.** Tested baseline: ESPHome 2026.8.2;
the regular renderer uses LVGL 9.5.

- **JC3248W535CN:** physical launcher and touch navigation confirmed; initial
  primary Wi-Fi scan/save/rollback checks passed.
- **NodeMCU-32S / ST7735:** 160×128 color launcher photographed on hardware;
  OTA installation, encrypted API reconnection and live MQTT reception verified.
  Build resource figures and remaining checks are in the
  [target notes](hardware/nodemcu-32s-st7735.md).
- **T-Call / SSD1309 OLED:** 128×64 monochrome encoder assembly; OTA/API verified
  and physical operation confirmed by the owner. [Device and evidence](hardware/tcall-ssd1309.md).
- **Host profiles:** 480×320 regular, 320×480 portrait, 160×128 compact color
  and 128×64 tiny/readable.

M1 adaptive profiles and M2 local forms have a host baseline. M3 remains open:
physical usability, recovery and prolonged reconnect/navigation testing are not
complete. Broader OLED qualification, generalized joystick/input adapters, BLE
cooperation, Nabla Edge integration and vehicle telemetry remain planned.
A working demonstration is not a completed hardware qualification.

## Repository support files

requirements.txt pins the ESPHome build dependency. .cursor/environment.json
selects the cloud bootstrap in .cursor/install.sh; contributor rules live in
.cursor/rules/. The camera service manifest is described in its
[service guide](services/homeassistant/README.md). Generated assets and retained
upstream sources keep their original contents and are explained by their module
README rather than edited solely to add comments.

## How it fits together

- **Device YAML:** application tree, hardware, private bindings and secrets.
- **packages/, components/, profiles/:** reusable composition, presentation and editors.
- **navigation/ and external_components/:** navigation contracts, code generation and adapters.
- **theme/, locales/, assets/:** visual style, translations and licensed assets.
- **simulator/ and tests/:** development fixtures and regression checks.
- **docs/platform/:** architecture, proposed capabilities and completion gates.

ESPHome packages assemble the firmware; navigation validation happens during
normal ESPHome compilation. There is no mandatory standalone generation step.
See the [component catalog](components/README.md) and
[navigation contract](navigation/README.md).

## Documentation and contributor language

Project documentation, comments, help and new identifiers use English. UI locale
resources may provide matching English/Spanish translations; user names and
bindings remain user-owned. Conversations and explicitly Spanish issue threads
may use Spanish. Source/configuration files should explain their purpose in a
short English header; preserve shebangs and third-party/generated content.
See [contributor rules](AGENTS.md).

## Design principles

Keep application meaning separate from screen layout and transport. A small
screen needs a readable layout, not a shrunken desktop. Touch actions need an
equivalent path through sequential input and Back.

Represent unknown, stale and unavailable data explicitly. Keep network work
bounded and the UI responsive. Add capabilities as documented modules with
examples and measured evidence.

A small encoder device may eventually control a larger panel, and a larger
panel may provide a keyboard for a smaller one. Those peer roles are a design
goal, not a released BLE feature.

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

The hardware photo above was provided by the project owner for publication.
It illustrates a private composition; it does not distribute its configuration.

## Contributing

Start with [AGENTS.md](AGENTS.md), the [architecture](docs/ARCHITECTURE.md) and
[brand guidelines](docs/BRAND.md). Documentation and identifiers use English;
the UI supports Spanish and English.

Preserve bundled font licenses and upstream component notices. In particular,
the optional captive portal retains ESPHome and webserver licensing.

## References

- [ESPHome LVGL](https://esphome.io/components/lvgl/)
- [ESPHome packages](https://esphome.io/components/packages/)
- [SDL host display](https://esphome.io/components/display/sdl/)
- [ESPHome UI Kit](https://github.com/mplogas/esphome-ui-kit)
- [ESPHome Modular LVGL Buttons](https://github.com/agillis/esphome-modular-lvgl-buttons)

External projects are references, not claims of API compatibility.

## Browser interfaces and planned menu studio

Optional [web menus](external_components/nabla_web/README.md) serve headless
camera devices. [Display mirroring](external_components/nabla_display_mirror/README.md)
serves monochrome and RGB332 frames from compact displays and LVGL. The
[web coordinator](external_components/nabla_web_service/README.md) selects
responsive, mirror or both views; omit these components to build without them.

[Nabla Control](https://github.com/txemavs/nabla-hacs) owns the Home Assistant
integration and consumes the [HTTP contract](docs/platform/DISPLAY-MIRROR-CONTRACT.md).
Device menus, rendering and firmware stay here. The [local equipment gallery](studio/README.md)
selects live devices; cross-profile simulation and a visual YAML editor remain
planned in the [studio roadmap](docs/platform/WEB-SERVICE-AND-STUDIO.md).
No hosted studio URL is advertised.
