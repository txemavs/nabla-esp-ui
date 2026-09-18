# Private device checklist

This checklist covers the configuration decisions for a new private device YAML
that consumes the public library. Real SSIDs, broker addresses and credentials
stay in your installation's `secrets.yaml`, never in this repository.

## Before you start

- [ ] Identify the hardware: board, display controller, touch/encoder inputs.
- [ ] Find or create a matching `hardware/*.yaml` adapter in the public library.
- [ ] Choose a profile: `packages/regular.yaml` (LVGL) or `packages/compact.yaml`.
- [ ] Select a tested library revision (full SHA, not `main`).

## Root YAML structure

### 1. Pin the library revision

```yaml
substitutions:
  nabla_ui_ref: <full-commit-sha>
```

Use the same SHA for all package imports and external components. Mixing
revisions breaks builds or produces undefined behavior.

### 2. Set device identity

```yaml
substitutions:
  nabla_device_model: "JC3248W535CN"
  nabla_display_kind: "IPS LCD"
  nabla_display_monochrome: "false"
  ui_language: en

esphome:
  name: my-panel
  friendly_name: "Kitchen Panel"
  project:
    name: "nabla.my-panel"
    version: "1.0.0"
```

Use a stable, descriptive `name` that matches your MQTT `client_id` convention.
The `project` block enables Device Builder identification.

### 3. Import library packages

```yaml
packages:
  library:
    url: https://github.com/txemavs/nabla-esp-ui
    ref: ${nabla_ui_ref}
    refresh: never
    files:
      - packages/regular.yaml      # or packages/compact.yaml
      - hardware/jc3248w535cn.yaml # your board adapter
```

Do not import example menus or demo bindings. The device YAML owns its full
`nabla_navigation` tree.

### 4. Configure Wi-Fi with multiple networks

```yaml
wifi:
  reboot_timeout: 0s
  networks:
    - ssid: !secret wifi_primary_ssid
      password: !secret wifi_primary_password
      priority: 40
    - ssid: !secret wifi_home_ssid
      password: !secret wifi_home_password
      priority: 30
    - ssid: !secret wifi_fallback_ssid
      password: !secret wifi_fallback_password
      priority: 20
  ap:
    ssid: "${friendly_name} Setup"
```

Higher priority wins when multiple networks are available. Retain at least one
fallback you control for recovery. The `ap:` block enables captive portal if
the optional component is imported.

### 5. Configure OTA with stable address

```yaml
ota:
  - platform: esphome
    password: !secret ota_password

esphome:
  # ... other settings ...
  on_boot:
    priority: -100
    then: []

# For devices behind DHCP/Tailscale where mDNS may not resolve:
wifi:
  use_address: !secret panel_ip
```

`use_address` provides a stable IP for OTA uploads when discovery fails.
This is especially important for routed VPN setups and mobile devices.

### 6. Optional: encrypted native API

```yaml
api:
  encryption:
    key: !secret api_encryption_key
  reboot_timeout: 0s
```

Native API is optional. Panels can operate with MQTT only or with both
transports. Set `reboot_timeout: 0s` to prevent reboots when Home Assistant
is unavailable.

### 7. Optional: MQTT lights

```yaml
substitutions:
  nabla_mqtt_command_topics: >-
    std::vector<std::string>{
      "nabla/ha/site/ui/cmd/light/entity1",
      "nabla/ha/site/ui/cmd/light/entity2",
      "","","","","","","","","","","","",""
    }

packages:
  mqtt_lights: !include .nabla-ui/adapters/mqtt-lights/package.yaml
  light_0: !include
    file: .nabla-ui/adapters/mqtt-lights/binding.yaml
    vars:
      slot: 0
      state_topic: nabla/ha/site/ui/state/light/entity1

mqtt:
  broker: !secret mqtt_broker
  port: 8883
  username: !secret mqtt_user
  password: !secret mqtt_password
  certificate_authority: !secret mqtt_ca
  client_id: !secret mqtt_client_id
```

See [Nabla Net MQTT](NABLA-NET-MQTT.md) for protocol details.

### 8. Optional: captive portal

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/txemavs/nabla-esp-ui.git
      ref: ${nabla_ui_ref}
      path: external_components
    components: [captive_portal]
    refresh: never

captive_portal:
  compression: gzip
```

The portal becomes accessible when the device enters AP mode after failing to
connect to configured networks.

### 9. Define your navigation tree

```yaml
nabla_navigation:
  forms:
    # Your form definitions
  tree:
    key: home
    title: ${tr_main_menu}
    children:
      # Your menu structure
```

Copy the structure from `examples/github/panel.yaml` and customize. The device
owns its menu; library updates do not overwrite it.

## Secrets file

Keep `secrets.yaml` in your ESPHome configuration directory, not in version
control. Example structure:

```yaml
# secrets.yaml (never commit)
wifi_primary_ssid: "MyNetwork"
wifi_primary_password: "correct-horse-battery-staple"
wifi_home_ssid: "HomeNetwork"
wifi_home_password: "another-secret-password"
wifi_fallback_ssid: "Backup"
wifi_fallback_password: "fallback-password"

ota_password: "ota-secret"
api_encryption_key: "base64-key-here"

mqtt_broker: "mqtt.example.net"
mqtt_user: "panel_user"
mqtt_password: "broker-password"
mqtt_ca: |
  -----BEGIN CERTIFICATE-----
  ...
  -----END CERTIFICATE-----
mqtt_client_id: "my-panel-01"

panel_ip: "192.168.1.100"
```

## MQTT client_id convention

Use a stable, unique identifier per device:

- Format: `<site>-<location>-<number>` or `<device-name>`
- Examples: `site-a-kitchen-01`, `site-b-pool-panel`, `garage-panel`

The broker uses `client_id` for connection tracking and ACLs. Two devices with
the same `client_id` will disconnect each other.

## Deployment checklist

Before first flash:

- [ ] Secrets file created with all required values.
- [ ] Library SHA tested (compile the public examples first).
- [ ] Hardware adapter matches your board.
- [ ] At least one Wi-Fi network you can access for recovery.
- [ ] OTA password set (even for USB-only initial flash).

After first flash:

- [ ] Device appears on expected Wi-Fi network.
- [ ] OTA upload succeeds (test with a no-op change).
- [ ] MQTT connects and receives state (if using mqtt-lights).
- [ ] Menu navigation works (touch or encoder, depending on hardware).
- [ ] Fallback AP activates when no networks are available.

## What NOT to commit

- `secrets.yaml` or any file containing real credentials.
- Device YAML with hardcoded SSIDs, passwords, IPs or broker addresses.
- `.esphome/` build directory.
- Firmware binaries (`.bin`, `.ota`).
- Private entity names that reveal your installation structure.

## Related documentation

- [Nabla Net MQTT](NABLA-NET-MQTT.md): protocol and topic structure
- [GitHub library](GITHUB-LIBRARY.md): remote package consumption
- [Private installations](PRIVATE-INSTALLATIONS.md): ownership boundary
- [Hardware adapters](../hardware/README.md): board-specific configurations
