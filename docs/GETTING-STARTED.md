# Getting started with Nabla ESP UI

This guide explains the library's role and the shortest path to a simulator or
a device-owned YAML composition. Project documentation is maintained in English;
UI text can still use the supported locale resources.

## What Nabla provides

Nabla is a common device UI built on ESPHome, with LVGL for regular displays and
the ESPHome display API for compact screens. Shared menus, navigation, forms and
appearance avoid reinventing the interface for each device. Home Assistant,
MQTT and other services are optional adapters. Local menus remain usable when a
network service is unavailable; remote actions still need their service.

The library is not a ready-to-flash image or a public inventory of installations.
Your private YAML owns its menu, hardware, network configuration and bindings.
Public examples use synthetic names and !secret placeholders.

## How it fits into Nabla

- [Nabla ESP UI](https://github.com/txemavs/nabla-esp-ui): this device UI library.
- [Nabla Edge](https://github.com/txemavs/nabla-edge): site/router services.
- [Nabla Net](https://github.com/txemavs/nabla-net): public website/docs.
- [Nabla Linux](https://github.com/txemavs/nabla-linux): Linux node image work.
- [Nabla Inference](https://github.com/txemavs/nabla-inference): optional compute/AI services.
- [Shared captive portal](https://github.com/txemavs/nabla-esphome-captive): optional provisioning component.
- [Nabla laboratory index](https://github.com/txemavs/nabla): project index, not a runtime dependency.
- Home Assistant: optional installation-owned entity state and commands.

These boundaries do not imply that every cross-project integration is released.

## Try the simulator

Use Ubuntu 24.04 or WSL with WSLg, Python 3.12 and SDL2 development libraries:

```sh
cd nabla-esp-ui
sudo apt-get install -y libsdl2-dev build-essential python3-venv
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
./simulator/run.sh              # 480x320 landscape
./simulator/run.sh portrait     # 320x480 portrait
./simulator/run.sh tft160       # 160x128 compact color
./simulator/run.sh tiny         # 128x64 compact
./simulator/run.sh readable     # 128x64 readable
```

Up/Down selects, Enter opens and Escape returns. The root triangle switches
presentation. Try Settings > Connections > Wi-Fi for the simulated flow; this
host demonstration does not change real networks. See the
[simulator guide](../simulator/README.md) for full setup and available fixtures.

## Use the library from GitHub

1. Start with the [editable consumer example](../examples/github/panel.yaml)
   in your private ESPHome directory or Device Builder installation.
2. Pin nabla_ui_ref to a tested full 40-character commit SHA and use the same
   revision for packages, helpers and assets.
3. Declare nabla_navigation.tree and forms in your device YAML.
4. Import matching hardware separately and choose regular or compact packages.
5. Keep Wi-Fi, encrypted API, OTA and secrets in the private installation.

Follow the [GitHub setup](GITHUB-LIBRARY.md) for exact package/resource syntax.
Do not import an example menu into a real deployment: the device owns its tree.
Changing the library pin is a deliberate upgrade; retain the previous pin for
rollback. Existing appearance settings and working navigation are the baseline.

## Adapt a physical device

Follow the [device adoption guide](DEVICE-ADOPTION.md) for hardware composition,
profile selection and staged validation. The [device guide](../devices/README.md)
distinguishes complete firmware roots from illustrative fragments. Small encoder
and monochrome targets have the same documentation/evidence requirements as
the large touch panel. See the [documentation map](README.md) for topic ownership.

## Read the code

- packages/: public regular/compact entry points.
- components/: reusable shell, controls, forms and visual behavior.
- navigation/: menu, focus and navigation policies.
- profiles/: supported geometry and compact density choices.
- external_components/: ESPHome schemas, code generation and runtime adapters.
- adapters/: optional Home Assistant/MQTT data and commands.
- hardware/: board, display, input, power and bus composition.
- simulator/ and tests/: host fixtures and regression checks.
- theme/, locales/, assets/: shared appearance, translations and resources.

Files include purpose comments; module READMEs explain dependencies and limits.
JSON manifests and generated/vendor files are documented by their owning README.

## Current verification boundary

The published baseline uses ESPHome 2026.8.2 and LVGL 9.5 for regular rendering.
M1 adaptive host profiles and M2 form flows have a host baseline. Physical
JC3248W535CN touch and NodeMCU/ST7735 encoder results exist; M3 extended recovery,
usability and soak testing remain open. BLE cooperation and later platform work
remain planned. Some menu entries are placeholders; optional light/camera/Wi-Fi
modules have their own implementation and verification limits.

See [tested devices](DISPLAY-CATALOG.md#tested-devices), the
[component catalog](../components/README.md) and [roadmap](platform/ROADMAP.md).

## Contracts to keep nearby

- [Library contract](LIBRARY-CONTRACT-v0.1.md): public API and consumer responsibilities.
- [UI consistency](UI-CONSISTENCY.md): global appearance and shared ownership.
- [Architecture](ARCHITECTURE.md): implementation boundaries.
- [Private installations](PRIVATE-INSTALLATIONS.md): private menus and credentials.
- [Private device template](PRIVATE-DEVICE-TEMPLATE.md): installation checklist.
- [Contributor instructions](../AGENTS.md): language, workflow and verification.
