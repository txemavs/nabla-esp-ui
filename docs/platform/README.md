# Platform plan

Design baseline: 2026-09-16, implementation reviewed at bc22dcd.
This directory specifies planned work, not a released platform API.
The working entry point remains simulator/hello-world.yaml and the accepted
navigation schema remains external_components/nabla_navigation/catalog.py.

## Product promise

Define the device's purpose in YAML. Reuse interaction, presentation and
connectivity modules. A panel works locally, optionally integrates with Home
Assistant, and can cooperate with Nabla Edge and other panels.
A large panel can become the keyboard or remote control of a smaller device.

The goal is a stable, maintained core with additive modules, not a frozen
codebase or a browser runtime on a microcontroller. A new application should
require composition and entity mapping; a new capability should require one
documented extension, not edits throughout the core.

## Read in this order

1. [Architecture and contracts](ARCHITECTURE.md): boundaries, YAML and lifecycle.
2. [Components and input](COMPONENTS-AND-INPUT.md): catalog, profiles and editors.
3. [Connectivity and commissioning](CONNECTIVITY.md): Wi-Fi, passwords and peers.
4. [Vehicle and Nabla Edge](VEHICLE-EDGE.md): independent startup and telemetry.
5. [Edge configuration integration](EDGE-CONFIG.md): reviewed menus and BLE bridge.
6. [Delivery roadmap](ROADMAP.md): milestones with completion gates.
7. [Research notes](SOURCES.md): primary sources and version caveats.

[Draft configuration](platform.proposal.yaml) is illustrative, deliberately outside
examples/. Do not feed it to ESPHome: the proposed nabla_ui domain does not exist.
It separates application meaning from rendering, bindings and transport.

## Working now

- ESPHome 2026.8.2 / LVGL 9.5 host build, SDL2 480x320.
- One YAML tree, nested navigation, tile/list switch, sequential scroll and focus.
- Four software rotations; grid changes between 4x2 and 2x4.
- Touch and keyboard adapters; keyboard emulates encoder U/D/ENTER.
- Shared header/footer surface, logo motion, progress demo and desktop clock.
- Dark/light content, Font Awesome Solid/Brands and build-time Spanish/English.
- Catalog validation and generated C++ traversal tests.
- Integrated manual SSID/password demo under Settings > Connections > Wi-Fi.

## Still to build

128x64 and larger-OLED profiles, real encoder/joystick adapters, the full form catalog,
live network status, Wi-Fi commissioning, peer protocol, Home Assistant data
bindings, Nabla Edge adapter, and vehicle telemetry. The named desktop
applications mostly remain placeholders. No physical board is validated.
The current runtime still contains fixed IDs, eight row slots and example-sized
layout assumptions; these are migration work, not the final module architecture.

## Design decisions

- Local functions never wait for a server, Wi-Fi, VPN or provisioning.
- Every touch action has a sequential input equivalent.
- A readable small-screen page replaces a scaled-down large-screen page.
- Data and action capabilities are independent of Home Assistant.
- Remote peers exchange typed capabilities and commands, not screen pixels,
  downloaded C++ or arbitrary YAML to execute.
- Prefer established ESPHome/LVGL features; add thin tested adapters where needed.
- Real progress and availability are explicit. An estimate is labeled an estimate.
- Third-party compatibility and public releases are evidence-based.

## First next step

The manual Wi-Fi form is now integrated in the main simulator. Refine its
interaction, add deterministic scan and connection-result fixtures, then build
the equivalent compact character picker and 128x64 list profile.
See [the roadmap](ROADMAP.md) for M0-M8 and current completion status.

Peer roles do not depend on screen size: a tiny encoder controller may operate
a larger panel, and a larger panel may provide a keyboard for a smaller target.
Negotiate capabilities and explicit session roles; see [peer roles](CONNECTIVITY.md).

Vehicle-specific decision: BLE announces Pi readiness and supports bootstrap;
the ESP32 joins the Pi-provided Wi-Fi and receives speed/telemetry over Wi-Fi.
The Pi reads the vehicle adapter over Bluetooth. Tailscale-dependent functions
remain disabled until the required service/route is verified. See [vehicle topology](VEHICLE-EDGE.md).
