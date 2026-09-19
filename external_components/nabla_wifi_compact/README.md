# Compact Wi-Fi adapter

Same radio/preferences backend as `nabla_wifi`, without the LVGL dependency.
Use this component for compact-shell profiles (tiny, tft160) that use the
ESPHome display API instead of LVGL.

The adapter sets `nabla_forms::WifiFlow::backend` at setup. The compact Wi-Fi
renderer (`compact_wifi.h`) checks this pointer and uses real scan/connect
when available, falling back to the mock flow on host builds.

## Requirements

- `wifi:` with 1..8 network entries (fallbacks)
- `nabla_navigation:` for form infrastructure
- ESP32 target (uses ESPHome preferences and WiFiComponent)

## Example

```yaml
external_components:
  - source:
      type: local
      path: external_components

wifi:
  networks:
    - ssid: !secret wifi_ssid
      password: !secret wifi_password

nabla_wifi_compact:
```

The user flow is the same as `nabla_wifi`: scan networks, select, enter
password if protected, connect. Success saves the primary; failure or
cancel restores declared fallbacks.

## Simulator

Host builds have no real radio; the mock flow remains active even with this
component present. Test real Wi-Fi on physical hardware.
