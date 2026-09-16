# Delivery roadmap

Status: plan, no promised calendar dates. Complete each exit gate before calling
its feature supported. Continue simulator work while hardware details are unknown.

## M0 — Establish the contracts

Publish this design, replace contradictory contributor instructions and record
current behavior. Define schema/metadata ownership and a conformance checklist.
Keep the current working simulator intact.

Exit: docs distinguish working code, proposed syntax and open decisions; all
links resolve; no draft file is presented as runnable firmware.

## M1 — Adaptive shell and focus

Status: host baseline delivered. See [profiles](../../profiles/README.md) and
[M1 verification](M1-VERIFICATION.md). Hardware validation remains M3.

Extract reusable shell/layout measurements from hardcoded example dimensions.
Add named regular, readable and tiny profiles. Prototype 128x64 with a real
128x64 viewport, not merely a rotated 480x320 scene. Support list navigation,
scroll anchors, focus/edit modes and optional footer layout.

Exit: the same eight route keys work at 480x320, 320x480 and 128x64; touch,
keyboard and simulated encoder traces yield equivalent actions. No clipped
required controls, lost focus or unreachable Back. At 128x64 test a three-row
list and a large-text single-item variant. Record measured font legibility.

## M2 — Forms vertical slice

Status: host milestone delivered. See [M2 verification](M2-VERIFICATION.md)
and the [forms contract](../../components/forms/README.md). Real networking and
persistent settings remain M3; this does not complete every planned UI component.

Implement text/password, numeric, selection, checkbox/toggle, validation and
dialog primitives. Settings > Wi-Fi uses a mock scan adapter and the same form
on touch, five-way keys and U/D/ENTER. Add long SSIDs, accents, empty results,
duplicates, invalid input, connection failure, cancellation and retry fixtures.

Exit: typing and cancelling never commits; Apply commits once; every field and
Save/Cancel is reachable without touch. Secret values do not appear in logs or
screenshots. Reducing a viewport never hides the focused field behind a keyboard.
Native keyboard limitations are addressed by a tested optional layout component.

## M3 — First real standalone device

Status: started. The JC3248W535CN firmware compiles and was uploaded with verified
flash data on 2026-09-16. The owner confirmed the eight-icon launcher and touch
navigation; USB logs corroborated route changes. See [hardware notes](../../hardware/README.md).
Static Wi-Fi/OTA and encrypted API logs are verified. The optional primary Wi-Fi
adapter now passes initial on-device scan, invalid-password rollback, save and
forget/reconnect checks. Information shows live model/network/version/uptime.
A rotation-related watchdog was investigated; the grid update order is corrected
and host-tested, pending physical confirmation. See [investigation](M3-WATCHDOG.md).
Local data bindings, physical encoder/joystick adapters and the full exit gate
below are still pending.

Confirm JC3248W535CN board revision, flash/PSRAM, controller, touch, backlight and
pins. Add physical encoder and five-way adapters with debounce and hold/release.
Add a local sensor/light binding and actual Wi-Fi scanning/credential application.

Exit: power-cycle recovery, offline operation, invalid Wi-Fi rollback, disconnect
while editing and at least a 24-hour reconnect/navigation soak pass. Publish
memory, image/font size, boot-to-interaction and input-latency measurements.
No hardware claim based solely on a host build.

## M4 — Assisted commissioning

First test standard provisioning compatibility; then implement a large-panel
central/client and a small target device. Establish target identity, pairing,
consent, transport protection, bounded packets and an explicit save result.

Exit: a larger device configures an initially Wi-Fi-less smaller one over BLE.
Wrong target, rejected authorization, dropped packets, duplicate submission,
link loss, failed join and revocation are tested. Credentials are absent from
logs, public state streams and persisted controller history.

## M5 — Typed device cooperation

Implement capability discovery and allowlisted typed commands; then remote-input
sessions using six semantic actions. Add local override, release on disconnect,
session expiry and operation status. Do not start with screen mirroring.

Exit: the same remote function works with two controllers of different sizes.
Test both a tiny encoder controlling a large panel and a large keyboard
configuring a small target. Mixed local/remote input cannot leave a held action stuck. A peer reconnects
without duplicating an action. Provisioning permission does not grant control.

## M6 — Home Assistant and Nabla Edge

Implement independent adapters and one shared application across local, HA and
Edge sources. Reuse the owner-confirmed Nabla Edge router: Wi-Fi/Ethernet uplink
and NablaNet access point with Tailscale routing are existing capabilities,
not new UI-project deliverables. The Pi reports service readiness; no UI component shells out to
Linux or stores Tailscale administration credentials.

Exit: stopping HA leaves local controls usable. Rebooting the Pi marks only its
services unavailable. Tailscale authentication required, no internet, route
unavailable and ready states are independently demonstrable.

## M7 — Vehicle pilot

Inventory the actual CAN/OBD adapter, transport and vehicle protocol. Begin with
recorded/synthetic fixtures, then a parked read-only pilot with an allowlist.
Measure cold-start and degraded-network behavior. See VEHICLE-EDGE.md.

Exit: panel remains interactive before the Pi is ready; missing/stale telemetry
is explicit; power loss and reconnect do not generate unintended bus commands.
Estimated waits are labeled and can expire without freezing the local menu.

## M8 — Public v1

Freeze supported contracts only after the vertical slices have proven them.
Publish a component gallery with downloadable reproducible examples, a board
matrix, changelog, migration guide, issue templates and contribution guide.
Confirm repository licensing and keep third-party notices; choose the project
license with the owner rather than inventing one in this planning pass.

Exit: an independent contributor can build local-only and HA examples, add a
module without editing unrelated core code, and understand supported vs planned
hardware. Pin released packages; do not recommend mutable main for deployed panels.

## Test and release matrix

On each relevant change: schema fixtures, generated C++ tests, input traces and
host compilation. For presentation changes: image checks for dark/light,
es/en, long strings, rotation and the affected profiles.
For transport changes: loss/reorder/duplicate/timeout/disconnect and secret
redaction tests. For hardware releases: memory high-water mark, reconnect soak,
power-cycle persistence and minimum voltage/power behavior on the actual board.
Avoid tests that merely repeat implementation; test user-visible invariants.

Initial engineering targets, to measure and revise: visible input feedback
within 100 ms at the 95th percentile, local interaction within two seconds of
power-on on the reference device, no monotonic heap loss after repeated
open/close/reconnect cycles. These are goals, not current benchmark results.
Reserve measured peak headroom for concurrent BLE, display and network use.

## Decisions needed later, not blockers for documentation

Exact OLED models and resolutions; physical text-size preferences; CAN/OBD
adapter model and BLE vs Classic; existing NablaNet readiness/API interface; pairing UX
for headless targets; permitted credential-storage policy; project license.
Collect these before their dependent hardware milestones. Do not guess pins,
vehicle PIDs, peer UUIDs or production security guarantees.

## Current implementation batch

There are nine milestones, M0 through M8: the design baseline plus eight
delivery phases. They are completion gates rather than an inflexible order.
M0 is documented. M1 now has regular/portrait and native 128x64 tiny/readable
fixtures with shared catalog and tested focus policies.
M2 includes native/compact text editors, validated YAML number/choice/toggle
fields, transactional save/cancel dialogs and deterministic scan/connect fixtures.
Twelve test cases cover schema, navigation, editing, information and the optional
Wi-Fi adapter's asynchronous state/recovery behavior.

Current demo path: Settings > Connections > Wi-Fi. Enter SSID/password,
validate locally and cancel through the standard navigation. action: wifi_demo
remains a compatibility alias. action: wifi selects the same form: mock by default,
real with the optional nabla_wifi adapter on the physical LVGL composition.
The standalone password preview remains available for isolated keyboard work.

Next steps:
1. Use the host demo to collect usability feedback without changing the M2 contract.
2. Continue M3 hardware validation on the running JC3248W535CN.
3. Extend real Wi-Fi recovery coverage and add physical input/local data adapters.
4. Measure resource use, legibility, reconnect behavior and the hardware exit gates.
5. Progress through M4 provisioning, M5 peer control, M6 integrations, M7
   vehicle pilot and M8 public release using the gates above.

The remote Nabla Config menu can be prototyped with fixtures alongside M2;
its real BLE/Pi operation layer belongs to M4-M6. Do not require Home Assistant
or a ready Nabla Net router to operate the local settings interface.
