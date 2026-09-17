# MQTT light adapter

Optional experimental transport for the regular light panel. One device can
control multiple Home Assistant installations through a shared MQTT broker.
Native API is not required for light commands.

Import package.yaml with the regular shell and authenticated TLS mqtt settings.
Include binding.yaml per light with slot (0–15) and state_topic. Supply
nabla_mqtt_command_topics as a 16-element C++ string vector. Empty topics cannot
send. Bind menu commands to control_open(slot, title) and set
nabla_light_slot_base for each grid route.

## Protocol

State JSON requires state (on/off/unavailable/unknown), available (boolean),
dimmable (boolean), brightness_pct (integer 0–100), and ts (Unix seconds).
Publish retained authoritative state every 30 seconds and after changes.
SNTP must synchronize before states are accepted. States over 90 seconds old
or 5 seconds in the future are rejected.

Commands are non-retained JSON: state (on/off), brightness_pct (0–100).
The server must allowlist topic-to-entity mappings and validate payloads.
Do not use legacy receivers that interpret every payload as a toggle.

## Behavior and limits

Sixteen slots, eight cards per regular grid, and a bounded 900-byte event log.
Commands do not invent state. Matching authoritative state clears pending;
after 15 seconds without confirmation a timeout is logged. Disconnect and
90-second stale state disable controls. Touch and U/D/ENTER share the editor;
ESC cancels. Credentials and installation bindings remain private.

Uses the regular renderer; compact light grids remain unsupported.
Validated with model tests and ESP32-S3 compilation. Live broker and physical
verification are required for each installation.
