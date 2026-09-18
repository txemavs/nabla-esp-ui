# BLE discovery, vehicle startup and peer control

Status: proposal for review, not implemented firmware or accepted ESPHome YAML.
This plan refines [vehicle topology](VEHICLE-EDGE.md), [peer roles](CONNECTIVITY.md)
and [Edge configuration](EDGE-CONFIG.md). It does not complete M3–M6.

## Intended experience

The panel opens its local menu immediately. A persistent status item/footer
explains the selected system's progress; opening it shows individual dependencies.
Use “System / Sistema” as the default user-facing name, with a configurable alias.
Do not require the user to understand Raspberry Pi, Bluetooth profiles or Tailscale.

Suggested Spanish messages, with English locale counterparts during implementation:

- **Esperando sistema**: no live, trusted status yet. Show elapsed waiting time.
- **Sistema detectado**: a compatible BLE advertisement was observed; identity
  and status still need verification.
- **Vincular sistema**: the target is new and requires pairing/authorization.
- **Sistema arrancando**: the authenticated service explicitly reports starting.
- **Preparando NablaNet**: AP or the selected startup readiness policy is pending.
- **Conectando a NablaNet**: the panel is attempting Wi-Fi association.
- **Comprobando servicios**: Wi-Fi is up; required endpoints are being checked.
- **Sistema listo**: the configured capability's dependencies have been verified.
- **Acceso remoto pendiente**, **Necesita conexión de subida**, **Autorización
  requerida**, **Sin respuesta**: actionable degraded states, not an endless
  “starting” animation.

Before the Pi's first announcement there is no evidence that it is powered or
booting. Never manufacture “Raspberry starting” from a timer. Do not invent a
percentage or a 30-second promise; use elapsed time and optional producer-supplied
estimates. Show determinate footer progress only for measurable work.
Local menus and settings remain available throughout.

“NFC” is not a required stage: these boards have no NFC hardware established in
this project. If NFC was intended, it can later supply an out-of-band pairing
reference; the startup/discovery path here is BLE.

On the large panel use the existing header/footer and a details page. On tft160
show one short status and scrollable details; on 128x64 prefer a short line or
single-item view. Indicators must work in monochrome without relying on color.

## Transport responsibilities

1. **BLE advertisements:** discover nearby candidates and hint at capabilities or
   a request for setup (“needs Wi-Fi”). They are observable, unauthenticated hints.
2. **Authenticated connected BLE / GATT:** read detailed startup state, negotiate
   capabilities, provision with consent and exchange bounded remote-input commands.
3. **Wi-Fi through NablaNet:** application data, camera images and vehicle telemetry.
   Existing MQTT remains an independent optional control transport.

A beacon does not execute a command, grant permissions, carry passwords, or prove
that a service is reachable. This first version is direct peer discovery, not
Bluetooth Mesh, automatic routing or rebroadcasting other devices' commands.

The Pi already provides NablaNet and Tailscale routing. Reuse that backend.
CAN/OBD acquisition remains Pi-side over the actual adapter's Bluetooth transport;
speed reaches the panel over Wi-Fi. Do not introduce BLE speed streaming.

## Existing Edge integration first

The earlier public review is recorded in EDGE-CONFIG.md; it is not evidence of
the installed Pi implementation. Before implementing a new protocol:

- Inventory the running BLE daemon, advertised UUIDs/data, BlueZ version, radio,
  service ordering and existing “needs Wi-Fi” announcements.
- Identify typed readiness signals for AP, uplink, routing, Edge API and intended
  destinations. Reuse existing state producers rather than scrape terminal menus.
- Determine whether an existing authenticated provisioning service can be reused.
  Document compatibility with its current consumers.
- Publish a sanitized contract/fixture and explicit protocol version. Only then
  assign UUIDs and choose the ESPHome-supported BLE stack for each board/framework.

If the current beacon format differs, add a bounded adapter or a negotiated new
version; do not silently break deployed devices. Keep Pi changes in Nabla Edge
and shared UI/controller code in this repository.

## Discovery proposal and resource budget

Use a custom 128-bit service UUID, with a compact service-data payload.
Do not borrow another vendor's manufacturer ID. Avoid duplicate UUID fields or
a mandatory local name in the primary advertisement.

Candidate legacy packet budget, to verify on the actual controller:
3 bytes Flags + 18 bytes service-data framing/UUID + at most 8 data bytes = 29 bytes.
The eight bytes could hold version (1), capability/status hints (2), rotating
discovery handle (4), and change counter (1). This is a proposal, not a frozen
binary specification. Full identity, boot ID, names and status belong in GATT.
A short handle may collide and is never an identity or authentication credential.

Initial tuning targets, not guaranteed platform capabilities:

- Advertise approximately every 0.5–1 second while starting/setup is requested;
  relax to 2 seconds when idle. Coalesce state changes.
- Bound the candidate cache to eight devices with expiry and bounded names.
- Scan in scheduled windows and reduce duty after connection; never run a blocking
  scan or reconnect loop on the UI task.
- Prefer the explicitly selected, paired system. RSSI is a discovery aid, not
  distance proof. Do not automatically replace an unavailable selected Pi with
  an unknown, stronger beacon. Multiple Pis get a chooser.
- Initially allow one active control session per small panel, retaining passive
  discovery. Benchmark before raising concurrency.

Verify the emitted packet length, API support, discovery latency and coexistence
on both ESP32 and ESP32-S3. BlueZ advertising properties are not a continuously
mutable status channel: use GATT notifications for live status and a controlled
advertisement re-registration only where the installed API requires it.

## Session and readiness contract

Proposed GATT responsibilities:

- Identity/capabilities: protocol versions, authenticated device identity,
  boot_id, supported role/capability IDs and size limits.
- Status snapshot plus notifications: independent services, state, sequence,
  producer uptime, error code and optional estimate.
- Authorized request/result: request_id, operation, bounded typed arguments,
  session/boot binding, accepted/running/succeeded/failed result.

Each service has unknown/starting/ready/degraded/error/unavailable plus local
last-received monotonic time. Track AP, uplink, Edge, VPN, destination and vehicle
adapter independently. BLE presence alone must never overwrite verified IP
reachability. Use fresh snapshots after reconnect.

Provisional limits: one outstanding request and one bounded 1 KiB reassembly
buffer per session; negotiated MTU-aware fragments, sequence/index/length checks,
2-second incomplete-frame timeout. Baseline fixtures must work with the minimum
negotiated payload; do not assume large MTUs. Define the final codec/framing in
the protocol implementation PR, with malformed-frame tests before enabling writes.

Start with a 2-second status heartbeat, mark stale after 6 seconds and disconnected
after 15 seconds; tune through measurements. A changed boot_id invalidates sessions,
pending results and input holds. Loss of BLE does not force Wi-Fi disconnection
when an independently verified IP service is still healthy.

The Pi bootstrap service starts after the Bluetooth controller and its own minimum
dependencies, without waiting for internet, MQTT, HA or VPN. It reports later
dependencies asynchronously. It cannot advertise before the radio is usable.

## NablaNet handover and service gating

Retain the agreed vehicle policy: on initial boot, the panel attempts the
Pi-provided Wi-Fi after authenticated readiness indicates AP + Edge + the required
Tailscale path are ready. That policy is explicit and configurable, not inferred
from the SSID. A future local-only policy needs an explicit configuration choice.

Reuse saved credentials. If missing, open an authorized commissioning flow over
the connected link; never broadcast them. Preserve the existing manual primary
and fallback networks, cancellation and rollback contract.

After association, verify the selected service with its real authentication and
intended route. A successful association, internet probe, MQTT broker or Tailscale
daemon alone does not establish Home Assistant availability.

Once connected, evaluate capabilities separately: losing VPN disables remote HA
actions while a healthy local telemetry endpoint can continue. Losing a single HA
does not disable local settings or all other services. No credentials or tailnet
administration tokens are needed in beacons.

## Pairing and control between peers

Controller and target roles are capability-based, independent of screen size:

- A small encoder controls an approved larger panel.
- A larger panel supplies text entry to a smaller Wi-Fi-less device.
- A panel reads/configures an approved Pi through the same bounded operation layer.

Use the platform's supported secure pairing/bonding and encrypted GATT, plus
application capability authorization. Confirm identity on both devices using
numeric comparison/passkey where supported, or a deliberate physical/OOB enrollment
procedure for devices without suitable IO. Do not treat proximity or unauthenticated
“Just Works” as permission to transfer credentials or control equipment.
Select the concrete pairing mechanism during the hardware audit; no custom crypto.

Discovery is open; status access, input control and network provisioning are
separate grants. Show target identity, connection state and a local Exit action.
Support revocation. Only deliberately approved devices reconnect automatically.

Negotiate semantic actions using navigation/INPUT.md. The target owns focus and
executes its own menu; do not transmit pixels, YAML, entity names or shell strings.
Remote input includes session/boot ID and sequence, with duplicate rejection.
Rotation steps may be coalesced under backpressure; never silently drop confirm
or release events. Local input takes priority and can terminate the session.

Before any held-input support: implement release-all on disconnect, a short
renewed input lease and target-side timeout (initial test target: at most 1 second).
Configuration requests are idempotent by request ID; ambiguous outcomes are queried,
not blindly replayed after reconnect. A BLE acknowledgement is not an operation result.

## Implementation phases and review gates

These are work packages inside the existing milestones, not a replacement roadmap.

### B0 — Audit and contract (M3/M4 prerequisites)

Inspect the installed Pi and both panel/framework combinations. Record existing
beacons, commissioning implementation, BlueZ capabilities and CAN radio use.
Deliver sanitized fixtures, compatibility decisions, UUID/version registry and
pairing/stack choice. Gate: two previously deployed consumers remain compatible.

### B1 — Offline readiness model and UI (M3)

Implement a pure bounded state/dependency model and simulated timeline:
no Pi, discovered, pairing, starting, AP pending, VPN pending, ready, reboot/error.
Deliver regular/tft160/tiny views and es/en strings. Gate: stale and reconnect
traces pass; local navigation never blocks and unknown is not displayed as zero.
This is the first implementation PR after accepting this plan.

### B2 — BLE read-only discovery (M4 groundwork)

Add the opt-in ESPHome discovery adapter and a Pi fixture/advertiser. Deliver
bounded cache, chooser, expiry, selected identity and version rejection.
Gate: multiple nearby devices, duplicates, malformed frames and radio load pass;
no command or credential transfer exists in this phase.

### B3 — Authenticated Pi status (M4)

Implement/reuse the Pi bootstrap service and connected GATT client, bonding and
status snapshots. Gate: real cold boot, slow/missing services, rejected identity,
lost notifications and new boot_id produce correct UI transitions.

### B4 — NablaNet handover (M3/M4)

Connect through the established Wi-Fi adapter, preserve fallbacks and verify the
actual local/remote service. Add consent-based credential provisioning only once
the pairing gate passes. Gate: bad password, no phone uplink, VPN authorization,
route failure and rollback cannot strand the local panel.

### B5 — Bidirectional peer control (M5)

Implement capability-scoped sessions, target indication, cancel/exit, deduplication
and release watchdog. Gate: both small-to-large input and large-to-small entry
work; reconnect cannot repeat a command or leave a held key active.

### B6 — Vehicle coexistence and endurance (M3/M6/M7)

Test the Pi acquiring CAN adapter data while serving BLE and NablaNet; test
ESP Wi-Fi traffic plus BLE discovery/control with camera and MQTT workloads.
If the Pi radio cannot meet measured requirements, evaluate a dedicated second
adapter rather than promising simultaneous performance.
Gate: parked bench/pilot tests, cold power cycles and 24-hour reconnect soak,
with measured heap/flash, boot-to-interaction, p95 input latency, discovery time
and stale-state deadlines. Define pass thresholds before each run.

## Test inventory and acceptance

Required automated traces: absent Pi, prolonged boot, repeated/out-of-order hints,
unknown protocol, cache overflow, identity collision, authenticated state expiry,
Pi reboot, missing fragments, Wi-Fi failure/rollback and independent VPN/HA loss.
Control tests cover wrong target, unauthorized requests, revoked bond, duplicate
request IDs, loss during Apply and forced release. Never log secrets in fixtures.

Required physical evidence covers both existing boards and the chosen Pi:
read-only startup first, then explicitly selected test actions. Record resource
deltas against the current MQTT firmware; do not infer available heap from static
RAM figures. Keep BLE optional so devices not using it pay no runtime cost.

## Primary implementation references

- [BlueZ advertising API](https://bluez.readthedocs.io/en/latest/advertising-api/):
  structured advertising, legacy payload budget and registration behavior.
- [BlueZ GATT API](https://bluez.readthedocs.io/en/latest/gatt-api/):
  service registration, reads/writes/notifications and negotiated MTU handling.
- [Espressif RF coexistence](https://docs.espressif.com/projects/esp-idf/en/v5.0/esp32/api-guides/coexist.html):
  shared-radio constraints; recheck against the target's pinned IDF version.
- [Existing Edge review](EDGE-CONFIG.md): reviewed source versus installed behavior.

The timing, budgets and phase gates above are design choices to validate, not
claims that the current firmware or installed Pi already implements them.
