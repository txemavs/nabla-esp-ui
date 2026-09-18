# Optional captive portal override

This is the working captive portal override migrated from an existing ESPHome
installation. Python, C++ and DNS files match ESPHome 2026.8.2 byte-for-byte.
The embedded web page differs from that release; it lists discovered Wi-Fi
networks, selects an SSID and provides password entry and OTA upload.

No SSIDs, passwords, device addresses or installation settings are bundled.
Network names and device information come from the device's /config.json.
The gzip and Brotli variants decode to the same index.html included here.
The compressed header is retained unchanged to preserve the working behavior.

## Consumer YAML

~~~yaml
external_components:
  - source:
      type: git
      url: https://github.com/txemavs/nabla-esp-ui.git
      ref: <tested-commit-sha>
      path: external_components
    components: [captive_portal]
    refresh: never

wifi:
  # Preserve your existing networks and priorities.
  ap:
    ssid: "Panel Setup"

captive_portal:
  compression: gzip
~~~

Import only captive_portal from this source; it replaces ESPHome's built-in
component. This is independent of the UI and does not require Home Assistant.
It becomes accessible when the device starts its fallback AP. Importing it
does not alter network credentials or Wi-Fi priorities.

Validated baseline: ESPHome 2026.8.2, NodeMCU-32S / Arduino, ST7735 panel.
Do not assume compatibility with older/newer ESPHome internals without compiling.
A pinned revision is recommended; the device YAML owns that pin.

## Licensing and origin

- ESPHome 2026.8.2: https://github.com/esphome/esphome/tree/2026.8.2/esphome/components/captive_portal
- The original generated page identifies https://github.com/esphome/esphome-webserver
  as its source. The precise local modification history is unavailable.
- LICENSE-ESPHOME retains ESPHome's complete license notice: runtime C/C++
  under GPLv3, Python under MIT.
- LICENSE-WEBSERVER retains the web project's MIT license and attribution.
These upstream licenses apply to this directory, independently of the UI library.
