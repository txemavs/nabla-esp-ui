# NodeMCU-32S + ST7735 + rotary encoder

Compact 160x128 landscape profile (tft160), native ST7735 panel 128x160.
The display driver owns rotation=270; do not rotate again in the renderer.
ESP32 Arduino, no PSRAM/LVGL requirement. Render the shared CompactShell with
the compact font triplet; initialize four rows and 16 px header/footer.

Known wiring from the working board:
- SPI clock GPIO18, MOSI GPIO23; CS GPIO5, DC GPIO16, reset GPIO17.
- Backlight LEDC GPIO4, 1000 Hz, active high.
- Encoder A GPIO32, B GPIO33, pull-ups, resolution 1.
- Encoder push GPIO27, K0 GPIO14, active-low with pull-ups.
- Encoder clockwise -> move(1), anticlockwise -> move(-1), push -> activate(),
  K0 -> back(). Keep the existing direction/pin configuration when migrating.

Import packages/compact.yaml with compact_profile: tft160. Keep the device's
navigation, pins, Wi-Fi, encrypted API, OTA and captive portal in its own YAML.
Do not copy credentials or a different device's API key when making test builds.
The optional [captive portal](../external_components/captive_portal/README.md)
is now available as a pinned Git external component; no local HA copy is needed.

Four scrollable list rows, a persistent footer and one-row readable mode (root
triangle activation). Appearance controls share dark/light, font family and
border inversion behavior with compact OLEDs. The default compact palette is high contrast; optional color.yaml supplies
Font Awesome icons with per-node colors and a 2x2 root launcher.

The Wi-Fi and generic form prototypes retain their original 128x64 geometry:
do not expose those demo actions on this profile as real network settings.
Display network status from the real Wi-Fi sensors; keep actual provisioning
in the existing captive portal until a hardware-backed form adapter is ready.
Compile/runtime verification is recorded with the private installation.


Optional color.yaml gives a 2x2 launcher and one-icon view. device-info/package.yaml
provides model, Wi-Fi/IP, router and RSSI through info fields. Long details scroll
with the encoder. MQTT compact.yaml uses the same 16-slot contract as the larger
panel, with an explicit brightness confirmation and no direct HA dependency.


## Verification snapshot — 2026-09-18

- Owner supplied a working display/encoder board and its existing ESPHome YAML.
- Nabla UI 0.6.0-mqtt compiled with ESPHome 2026.8.2 and installed by OTA;
  encrypted API reconnected and reported the new project version.
- MQTT connected and real available-state reception was observed. No light
  command was sent by the verification probe.
- The private device YAML is editable in ESPHome Builder; UI, fonts and optional
  captive portal are fetched from pinned GitHub revisions. Consumer compilation
  passed with remote resources after fixing an asset-root precedence error.
- The owner supplied a [photo of the color launcher on hardware](../docs/images/nabla-panels-touch-and-encoder.png).
  The layout was also inspected in the native-size SDL fixture; 24 automated
  tests passed, including draft cancellation, disconnect and range limits.
- That MQTT build used 1,185,535 bytes of application flash (64.6% of its OTA
  partition) and 62,068 bytes of static RAM (34.3% of the linker-reported region).
  These are build figures, not runtime heap or latency measurements.

Still required: physical legibility/usability checks and encoder direction confirmation, on-device brightness/Cancel checks, captive-AP recovery after the
source migration, power-cycle/reconnect coverage and a 24-hour soak.
The source migration alone does not retest captive provisioning or complete M3.


## Real Wi-Fi and encoder password entry (2026-09-20)

The [public device composition](../devices/nodemcu-32s-st7735.yaml) connects
the compact Wi-Fi form to nabla_wifi_compact. The Wi-Fi menu must declare
action: wifi; a descriptive leaf alone cannot scan or edit credentials.
Declare fallback networks and retain their existing priorities. The adapter
stores a successfully connected primary network and rolls back on cancellation
or failure. Never replace installation networks with example credentials.

Rotate the encoder to move, press to select, and use K0 to return. The shared
compact password editor keeps the password masked and supplies character entry;
this is an on-screen encoder keyboard, not Bluetooth keyboard support.
The same compact renderer owns the logo, Back behavior and animations.

The public hardware package now selects nodemcu-32s and ili9xxx/ST7735, matching
the deployed kit. Native dimensions are 128x160; driver rotation 270 yields
160x128 landscape. A 33 ms display interval allows the shared animations to
refresh more frequently; smoothness still requires owner verification.

The public composition and a private composition compiled with ESPHome 2026.8.2.
USB flash verification succeeded on an original ESP32 and the device reported
Wi-Fi connected after restart. Password entry, successful primary-network
replacement, cancellation, power-cycle persistence and appearance are pending
physical acceptance. No private networks, MQTT bindings or secrets are included.
Provide wifi_ssid and wifi_password in a private secrets file before compiling
the public root. It has no API/OTA setup; preserve those in deployed roots.
