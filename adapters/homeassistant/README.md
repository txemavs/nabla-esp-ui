# Home Assistant clock and Wi-Fi status

Optional regular/LVGL adapter. Import adapters/homeassistant/package.yaml from
the same pinned Git package as packages/regular.yaml. The device owns wifi,
encrypted api, OTA and credentials. Set nabla_timezone for its location.

Home Assistant's ESPHome integration must connect to the device. Merely adding
the YAML to Device Builder does not establish that integration.

The footer shows --:-- until a valid clock is available. Native Home Assistant
time synchronizes the device; no MQTT request/reply automation is needed.
After synchronization the clock continues locally if the connection is lost,
until reboot (subject to oscillator drift).

Wi-Fi icon: gray when disconnected, green when Wi-Fi is connected, blue when
an API client is subscribed to entity states. Log-only connections do not count.
This is ESPHome's integration connection test, not an HTTP URL health check or
proof of a specific server identity. Another state-subscribing API client also
qualifies. A disconnected subscription returns to green within one second.

Monochrome uses foreground for connected states and gray for disconnected;
color-only differentiation of Wi-Fi versus HA is not available on monochrome.
Bluetooth remains a placeholder.

See [site panels versus MQTT Control](../../docs/platform/HOME-ASSISTANT-PANELS.md) for the separate multi-instance URL-client contract. The native adapter above does not implement it.
