# Nabla Edge configuration from an ESP32

Status: source review and proposed integration; no remote configuration service
or BLE client has been implemented in this UI repository.
Reviewed public Nabla Edge revision: `20d2854e6c8bf93dec199faa011f3b6d3fd715ee`, 2026-09-16.
The installed Pi services and packaged binaries were not inspected.

## What can be reused

- [nabla-config](https://github.com/txemavs/nabla-edge/blob/20d2854e6c8bf93dec199faa011f3b6d3fd715ee/nablaedge/scripts/nabla-config) is a shell/whiptail frontend.
  Its main menu exposes Network, Media, Accessories, Camera, Voice Satellite,
  Info and Exit. Network exposes status and cable/ap/cable-ap/off modes.
  It delegates network operations to an external vpn-mode executable.
  The reviewed public tree does not contain that executable's source or a
  separately named vpn-config implementation.
- [nabla.menu/v1](https://github.com/txemavs/nabla-edge/blob/20d2854e6c8bf93dec199faa011f3b6d3fd715ee/protocols/menu/PROTOCOL.md) is explicitly a draft.
  It already describes YAML authoring, JSON delivery over MQTT, submenus,
  buttons, toggles, numbers, information and Back, with optional HA actions.
  Reuse its semantics through an adapter instead of creating an unrelated
  Edge menu format. This does not establish that a runtime is deployed.
- [Device contract](https://github.com/txemavs/nabla-edge/blob/20d2854e6c8bf93dec199faa011f3b6d3fd715ee/protocols/menu/device/CONTRACT.md) describes an MQTT
  renderer with rotary input and cached menus. It is not a BLE transport.
- [OLED profile](https://github.com/txemavs/nabla-edge/blob/20d2854e6c8bf93dec199faa011f3b6d3fd715ee/nablaedge/ui/ssd/profiles/128x64.yaml) supplies useful
  small-display layout references. Adapt these to our readable profiles,
  current brand and six-action input contract rather than copying coordinates.
- [BLE documentation](https://github.com/txemavs/nabla-edge/blob/20d2854e6c8bf93dec199faa011f3b6d3fd715ee/nablaedge/docs/ble-mesh/README.md) describes presence
  and state advertisements and says the external component will be published.
  No executable configuration-over-BLE service was found in the reviewed tree.
  Its advertised packet sketches are not a credential-transfer protocol.

The network documentation and shell entry points are not entirely aligned:
the network guide uses --network whereas the script accepts network, and the
guide's bridging description is not evidence of the owner's deployed routing.
Use the actual installed backend as the integration authority.

## One backend, multiple interfaces

The Pi owns configuration, validation, persistence and execution. The terminal
frontend and ESP32 frontend should invoke the same typed operation layer.
Do not scrape whiptail output or forward arbitrary shell strings from a panel.

The ESP32 owns rendering, breadcrumbs, focus and editors. The same configuration
tree becomes tiles on a large display or a scrolling list with UP/DOWN/ENTER
on a small OLED. Back/Cancel remains reachable without a physical ESC key.

Proposed operation names, not existing APIs:
- network.status: current mode and independent uplink/AP/Tailscale observations.
- network.set_mode: validated enum cable/ap/cable-ap/off.
- wifi.scan and wifi.configure: later forms for the selected Pi interface.
- system.info: bounded read-only device and service information.

Operation responses distinguish accepted, running, succeeded and failed.
An acknowledgment is not success: verify the resulting backend state.
Keep an operation ID across reconnection and deduplicate writes. Configuration
authority stays on the Pi; privileged work uses narrow allowlisted operations.

## Boot and transport

Start the Pi's minimal configuration service once Bluetooth and its backend
dependencies are available; it must not wait for Wi-Fi, MQTT, HA or Tailscale.
An ESP32 cannot control an unbooted Pi: retain local navigation while waiting.

BLE advertising discovers the target. An authorized connected BLE session then
carries bounded menu/status requests, form submissions and operation results.
Define authentication, framing, MTU-aware fragmentation, maximum sizes and
timeouts before implementation. Never place passwords in advertisements,
retained menu messages or diagnostic logs.

The initial panel includes a local bootstrap/recovery menu. It must not need
to download its only configuration interface through the missing Wi-Fi link.
Later, capability-gated dynamic Edge menus can extend that local shell.

Once NablaNet is usable, MQTT can carry the existing menu format and Wi-Fi
carries telemetry. BLE remains useful for recovery when a network operation
interrupts Wi-Fi. Transport switching must not repeat an in-flight operation.
CAN speed remains Pi-to-ESP32 over Wi-Fi, not BLE.

## Compatibility work before claiming support

The UI's current compile-time navigation schema is not nabla.menu/v1.
Implement and test an explicit importer/adapter; do not feed remote YAML into
ESPHome or execute downloaded lambdas.

The draft needs coordinated extensions for stable node/action identifiers,
localization keys, text/password forms, target identity, request/result
correlation and transport-independent state bindings. state_topic and MQTT
actions need a defined bridge in BLE-only sessions, not implicit MQTT access.
Existing nabla internal commands must not silently become privileged Pi commands.

Publish the supported subset and resource limits. Unsupported major versions
must fail closed for actions. Validate a replacement menu before activating it;
retain a usable local fallback. These choices differ from permissive parsing
and discard-first suggestions in the draft and need agreement upstream.
Firmware updates remain necessary for new widget types or capabilities even
when ordinary menu content can be updated without reflashing.

## First demonstrable slice

1. Add a simulated Remote device > Nabla Config > Network menu using fixtures.
   Include status, the four mode choices, confirmation and operation feedback.
2. Render the same menu on the color simulator and 128x64 list profile.
3. Define a Pi operation facade around the installed network backend and test
   command failures, timeouts and actual state verification independently.
4. Connect an authorized BLE session with Wi-Fi disabled; read status and apply
   one mode on a bench Pi. Verify reconnect and duplicate-request handling.
5. Add Wi-Fi scan/password forms, then accessories and other capabilities.

Media imaging and other destructive maintenance actions are outside this first
slice; they require their own target identification and confirmation contracts.
Test Pi boot delay, unavailable backend, wrong peer, lost BLE, Wi-Fi loss during
Apply, Pi restart and stale status. Keep the existing router implementation.
