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
If using a custom captive portal, retain its implementation and verify the
source path relative to the device YAML before compiling.

Four scrollable list rows, a persistent footer and one-row readable mode (root
triangle activation). Appearance controls share dark/light, font family and
border inversion behavior with compact OLEDs. The TFT currently uses the
high-contrast compact palette. TFT color artwork can be added separately.

The Wi-Fi and generic form prototypes retain their original 128x64 geometry:
do not expose those demo actions on this profile as real network settings.
Display network status from the real Wi-Fi sensors; keep actual provisioning
in the existing captive portal until a hardware-backed form adapter is ready.
Compile/runtime verification is recorded with the private installation.


Optional color.yaml gives a 2x2 launcher and one-icon view. device-info/package.yaml
provides model, Wi-Fi/IP, router and RSSI through info fields. Long details scroll
with the encoder. MQTT compact.yaml uses the same 16-slot contract as the larger
panel, with an explicit brightness confirmation and no direct HA dependency.
