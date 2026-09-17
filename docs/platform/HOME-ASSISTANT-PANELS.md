# Home Assistant panels and MQTT Control

## Separate applications and transports

A site panel belongs to one Home Assistant instance. Its authoritative states,
availability and commands come from that instance's authenticated API.
The MQTT Control application is a separate application for independent remotes
and fallback use. It does not silently supply a site's Home Assistant panel.

The light card, brightness editor, focus and layout are shared presentation.
Sharing a widget must not share or replace its data source.

## Connection contract

Each Home Assistant connection owns a base URL, private credentials and status:
unconfigured, connecting, ready, unreachable, unauthorized or stale.
A successful web-page response or another server's connection is insufficient.
Ready requires successful authenticated API access to the configured server.

An unavailable site tile is gray. It may remain focusable to open an explanation
with the server URL and reason, but cannot open operational controls or execute
commands. Never include credentials in status text. Recheck availability when
sending a command. Preserve separate unavailable states for individual entities.

A private-network URL requires a working route from the device. A router can
provide that route; the ESP32 need not run a VPN client itself. MQTT availability
does not prove reachability of either Home Assistant URL.

## ESPHome native API versus URL clients

ESPHome's native API is an inbound connection from Home Assistant to the device.
A public Home Assistant frontend does not provide the reverse network path.
The existing homeassistant adapter detects state subscriptions without proving
which specific server subscribed.

Direct outbound access to a Home Assistant URL requires an authenticated HTTP
or WebSocket client for that instance. Use independently configured credentials,
TLS verification, bounded response handling and nonblocking network work.
Do not reuse browser login sessions or publish access tokens in library code.

## Migration status

The current MQTT light adapter is implemented and remains useful for Control.
Site-specific MQTT aliases were a temporary composition and do not satisfy this
direct-Home-Assistant contract. Do not describe them as direct API panels.
The experimental nabla_ha HTTPS adapter and ha-lights integration implement this
separation with per-instance credentials and availability. Physical deployment
must verify both URL routes and authentication; compilation alone is insufficient.

Acceptance: disconnect one site's route or revoke its credential; only that
site's panel becomes unavailable. MQTT Control remains independent. Restore the
route and verify authoritative state recovery without queued offline commands.
