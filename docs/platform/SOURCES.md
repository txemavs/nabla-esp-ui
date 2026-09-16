# Research and compatibility notes

Reviewed 2026-09-16. Repository baseline: ESPHome 2026.8.2, LVGL 9.5.
Online documentation may describe newer releases (the public widget page already
shows 2026.9.0); compile and inspect the pinned source before adopting any API.
This research did not update dependencies or configure radios/services.

## Existing building blocks

[ESPHome LVGL widgets](https://esphome.io/components/lvgl/widgets/) documents
textarea and keyboard primitives. Keyboard modes and ready/cancel events provide
a starting point, but arbitrary custom keyboard layouts are not supported by
that wrapper. Locale-specific layouts need a separately validated extension.
The installed textarea and host-time modules were also inspected.

[ESPHome Wi-Fi](https://esphome.io/components/wifi/) provides network management.
The application must adapt actual supported operations rather than assume a
scan/rollback API exists with the desired semantics.
[ESPHome FAQ](https://esphome.io/guides/faq/) and the Wi-Fi reference describe
connection reboot policies. Local-only designs must explicitly review Wi-Fi/API
timeouts and choose recovery behavior; disabling a timeout is not a complete
recovery strategy.

## Provisioning and Bluetooth

[ESPHome Improv BLE](https://esphome.io/components/esp32_improv/) implements
credential reception on ESP32 and exposes authorization/provisioning events.
It also notes the BLE stack's significant RAM use. A large-panel provisioning
client is separate work, not provided by enabling the receiver.
The installed 2026.8.2 component was inspected for its schema and dependencies.

[Improv BLE specification](https://www.improv-wifi.com/ble/) defines states,
capabilities and RPC messages, including optional operations added by revisions.
Negotiate capabilities rather than infer them from the advertised name.
Physical authorization is not evidence of encryption or authenticated identity.
The security suitability of a chosen implementation must be validated separately.

[ESPHome Bluetooth proxy](https://esphome.io/components/bluetooth_proxy/) is a
Home Assistant integration path, not a ready-made arbitrary peer command protocol.

[Espressif ESP32-S3 Bluetooth overview](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/ble/overview.html)
states that Classic Bluetooth is not supported. This matters for purchased
vehicle adapters; verify BLE vs Classic before selecting the direct-panel path.
Do not generalize a chip capability to a tested ESPHome multi-role configuration.

## Pi readiness and vehicle telemetry

[Tailscale CLI](https://tailscale.com/docs/reference/tailscale-cli) documents
status reporting, including structured output. The Edge adapter should report
specific states rather than equate process startup with remote reachability.
[Tailscale up](https://tailscale.com/docs/reference/tailscale-cli/up) concerns
connection setup and authentication; keep these administrative operations on
the Pi and outside untrusted peer commands.

[ESPHome CAN bus](https://esphome.io/components/canbus/) supplies frame-oriented
building blocks. It does not identify the user's adapter, decode every vehicle,
or make active OBD polling equivalent to passive listening.

## What is a proposal here

Module metadata, schema vocabulary, profile sizes, performance targets, peer
envelope, security policy and commissioning rollback are project design proposals.
They are not features claimed by the linked upstream projects. They need the
implementation and exit gates in ROADMAP.md.
