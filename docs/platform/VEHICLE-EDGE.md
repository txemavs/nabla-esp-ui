# Vehicle panel and Nabla Edge

Status: proposed deployment architecture, not an implemented car integration.
The Raspberry Pi 4, exact panel and purchased CAN/OBD adapter must be inventoried.
No existing Nabla Edge implementation or API was inspected in this planning pass;
its adapter contract must be mapped to the actual service before implementation.

## Responsibility split

The ESP panel boots its local UI, reads available local sources and renders
service state. It never waits synchronously for the Raspberry Pi.
The Pi runs Nabla Edge services, Tailscale and any heavier protocol/media adapters.
A local authenticated link carries typed state and commands between them.

For a fixed in-car installation, evaluate a wired local link first (UART/USB
where supported) because it can expose readiness before Wi-Fi exists.
If wireless is selected, the UI must distinguish 'Pi unknown because local link
is down' from 'Pi confirmed booting'. Exact framing and hardware pins remain
open until the hardware inventory. Tailscale is not required for the local link.

## Independent readiness state

Model panel, Pi heartbeat, Edge API, Wi-Fi uplink, internet, Tailscale and vehicle
adapter as separate services. Each publishes unavailable/starting/ready/degraded/
error, last observation age, error code and optional estimated wait.
Dependencies are capability-specific: local telemetry does not depend on VPN.

Example sequence:
- Panel becomes interactive; local menu, clock and supported sensors work.
- Pi has not announced itself: 'Waiting for Nabla Edge', with elapsed time.
- Pi announces boot ID and Edge status: report actual service startup.
- Uplink is absent: keep local tools usable and mark remote access unavailable.
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
To read while the Pi starts, the panel needs its own compatible independent path.
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
