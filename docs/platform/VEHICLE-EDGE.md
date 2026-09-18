# Vehicle panel and Nabla Edge

Status: proposed deployment architecture, not an implemented car integration.
The Raspberry Pi 4, exact panel and purchased CAN/OBD adapter must be inventoried.
The public menu script and protocol drafts have now been reviewed; see
[Edge configuration integration](EDGE-CONFIG.md). Installed Pi services and the
network backend still need to be mapped before implementation.

## Responsibility split

The ESP panel boots its local UI, reads available local sources and renders
service state. It never waits synchronously for the Raspberry Pi.
The Pi runs Nabla Edge services, Tailscale and any heavier protocol/media adapters.
A local authenticated link carries typed state and commands between them.

### Selected vehicle topology

The Pi reads the vehicle adapter over Bluetooth (actual adapter protocol still
to be verified), provides a Wi-Fi access point for the ESP32, and exposes
vehicle telemetry over that Wi-Fi link. Speed is not transported over BLE.

BLE is the bootstrap/control channel: reuse the existing Nabla Edge Pi
advertising/commissioning behavior after inspecting its installed implementation.
The public BLE documentation does not yet establish a configuration service.
The Pi announces readiness; the ESP32 then joins the Pi's Wi-Fi and subscribes
to the telemetry service. Previously provisioned credentials can be reused;
new credentials require the authorized provisioning flow, not public advertising.

The intended startup policy gates the ESP32's connection to the Pi's Wi-Fi on
Edge's readiness announcement, including the required Tailscale path. Until
then, dependent UI actions stay disabled while local functions remain usable.
An advertisement is a discovery hint, not proof of authentication or reachability:
after joining, verify the service and required route before enabling actions.

Keep readiness fields distinct: access point ready, Edge service ready,
Tailscale connected, and intended destination reachable. A Pi may delay starting
its AP or simply delay announcing it as ready; that implementation choice remains
in Nabla Edge. Losing VPN later must disable VPN-dependent actions without
necessarily discarding a usable local Wi-Fi/telemetry connection.

### Existing Nabla Edge router capability

Owner-confirmed deployment: Nabla Edge Pis already accept an upstream connection
over Wi-Fi or Ethernet and expose a NablaNet Wi-Fi network with working
Tailscale routing. The vehicle Pi has an additional Wi-Fi adapter: it can use
a phone hotspot as upstream while providing NablaNet to the ESP32.
The phone supplies connectivity; it does not itself need to participate in
Tailscale. The Pi owns the tailnet connection and routing.

Treat this as an existing integration dependency, not a router feature to
implement in the UI project. The routing configuration was reported by the
owner, not independently inspected in this repository. Inspect existing Edge
readiness messages and interfaces before writing the adapter; reuse them.

The ESP32 joins NablaNet and can reach the permitted Home Assistant instances
and other tailnet resources through the Pi. The UI needs credentials, readiness
state and service reachability; it does not implement Tailscale or routing.
If an individual service is unavailable, disable only the capabilities that
depend on it. Internet uplink, NablaNet association and HA availability remain
separate observations even though the router implementation is already working.

## Independent readiness state

Model panel, Pi heartbeat, Edge API, Wi-Fi uplink, internet, Tailscale and vehicle
adapter as separate services. Each publishes unavailable/starting/ready/degraded/
error, last observation age, error code and optional estimated wait.
Dependencies are capability-specific: local telemetry does not depend on VPN.

Example sequence:
- Panel becomes interactive; local menu, clock and supported sensors work.
- Pi has not announced itself: 'Waiting for Nabla Edge', with elapsed time.
- Pi announces boot ID and Edge status: report actual service startup.
- Uplink is absent: keep local tools usable and mark remote access unavailable;
  do not announce the Wi-Fi service as ready under the selected startup policy.
- Tailscale needs authentication: show that state, not a continuing countdown.
- VPN reports running: test the intended remote capability separately before
  showing it available. A daemon being active does not prove a route is usable.

The Pi adapter may inspect structured Tailscale status and a specific authorized
health endpoint. Tailscale installation, login and route policy remain Pi-side
administration, not privileged commands accepted from the panel.
See [Tailscale source notes](SOURCES.md).

A message such as 'about 30 seconds remaining' is an estimate learned or
configured for that service, not a promise or a percentage. When the estimate
expires, show 'taking longer than expected' with elapsed time and diagnostics.
Use determinate footer progress only when the producer supplies measurable work.
The existing one-second logo/progress sequence remains a visual intro.

Heartbeats include a boot/session ID and monotonic age. A Pi reboot invalidates
old operation acknowledgments. A missing heartbeat marks data stale after a
configured deadline, not immediately zero. Do not block the whole panel or
restart it because Edge, HA or Tailscale is offline.

## Vehicle data path

The word CAN does not identify the purchased adapter's protocol:
it might be an electrical CAN interface, an OBD-II interpreter or a bridge.
Before implementation record model, firmware, bus interface, power requirements,
Classic/BLE/USB transport, supported services and vehicle/protocol information.

ESP32-S3 supports BLE, not Bluetooth Classic. A Classic-only adapter may need
the Pi or another supported bridge. If the Pi owns that adapter, its telemetry
cannot be available before the Pi boots; show that limitation explicitly.
Under the selected topology, vehicle telemetry waits for the Pi and its Wi-Fi
service. Local UI remains available, but no pre-Pi speed reading is promised.
See [Espressif compatibility notes](SOURCES.md).

Phase one is read-only diagnostic display with recorded fixtures first.
A raw CAN decoder needs verified identifiers, lengths, scaling and units.
OBD polling is active request traffic even when the application only displays
readings; it is not equivalent to passive listen-only capture.
Use explicit supported parameter allowlists, bounded polling and stale markers.
Do not promise generic vehicle compatibility or derive unknown values as zero.

Actual bus writes, actuator control and ECU modification are outside the first
pilot. Validate power/transceiver isolation and bus behavior on the bench and
parked vehicle before any operational use. The UI should favor glanceable status,
large text and physical controls; credential editing belongs to setup while parked.

## Cooperative use cases

- Pi offline: panel still navigates, shows local values and readiness.
- Pi ready without internet: local Edge functions become available.
- VPN unavailable: remote functions show their dependency; local functions remain.
- Another small device needs Wi-Fi: large panel provides the commissioning UI.
- Joystick mode controls a nearby approved device with a visible target and Exit.
- HA absent: all configured local and Edge-independent behaviors continue.

## Failure and recovery fixtures

No Pi power, slow boot, failed service, clock unsynchronized, authentication
required, expired VPN session, unreachable route, car adapter disconnected,
radio contention, stale telemetry, malformed values and sudden power loss.
Log codes and state transitions without secrets or arbitrary raw vehicle traffic
by default. Persist only deliberate settings; avoid flash writes on each sensor
sample. Power recovery must never replay old remote commands automatically.

Success is not 'the dashboard looks connected'. Success is a responsive panel
whose individual capabilities accurately describe what can be done now.

Peer roles do not depend on screen size: a tiny encoder controller may operate
a larger panel, and a larger panel may provide a keyboard for a smaller target.
Negotiate capabilities and explicit session roles; see [peer roles](CONNECTIVITY.md).

## End-to-end speed display

Vehicle adapter -> Bluetooth -> Pi decoder/state -> Wi-Fi -> ESP32 UI.
The Pi publishes a typed speed value with units, validity, sequence and age.
Transport choice (for example an existing Edge API or MQTT subscription) waits
for inspection of the actual Edge service. Never infer speed from BLE readiness.
On telemetry timeout, mark the value stale/unavailable rather than retain an
apparently live speed or replace missing data with zero.


## Next implementation proposal

See [BLE startup and peer-control implementation plan](BLE-STARTUP-PLAN.md) for startup messages, discovery/session
boundaries, NablaNet handover and phased acceptance gates. It is a proposal,
not an implemented BLE service or supported firmware configuration.
