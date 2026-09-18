# MQTT lights example

This directory contains a synthetic device YAML demonstrating the `mqtt-lights`
adapter with invented topics and placeholder secrets. It illustrates composition
structure, not a directly compilable configuration.

## Files

- `panel.yaml`: Regular (LVGL) panel controlling 4 lights across 2 fictional sites.

## Purpose

Show how a private device YAML:
1. Pins a library revision.
2. Imports hardware, shell and the mqtt-lights adapter.
3. Binds state topics to slots.
4. Supplies command topics as a vector.
5. Wires menu commands to `control_open(slot, title)`.

## Building

This example requires a `secrets.yaml` with the following keys:

```yaml
wifi_ssid: "YourNetwork"
wifi_password: "your-password"
ota_password: "ota-secret"
mqtt_broker: "mqtt.example.net"
mqtt_user: "panel_user"
mqtt_password: "broker-password"
mqtt_ca: |
  -----BEGIN CERTIFICATE-----
  ...
  -----END CERTIFICATE-----
```

Copy `panel.yaml` to your ESPHome directory, create the secrets file, and
adjust topics to match your MQTT broker configuration.

## What is fictional

- **Topics**: `nabla/ha/nave/...` and `nabla/ha/villa/...` are invented.
- **Entity names**: "Living Room", "Kitchen", "Pool", "Terrace" are placeholders.
- **Broker address**: `mqtt.example.net` does not exist.
- **Device name**: `example-mqtt-panel` is not a real device.

Replace all of these with your private installation values.

## Related

- [Nabla Net MQTT](../../docs/NABLA-NET-MQTT.md): protocol documentation
- [Private device template](../../docs/PRIVATE-DEVICE-TEMPLATE.md): checklist
- [mqtt-lights adapter](../../adapters/mqtt-lights/README.md): implementation
