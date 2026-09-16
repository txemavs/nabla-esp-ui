# Device information

package.yaml registers a read-only runtime snapshot. The shared catalog includes
it; use info: model, wifi, ip, version or uptime on an ordinary leaf (no action,
static detail or children). Information is the first Settings child in the demo.
Menu rows use a short summary; opening a leaf uses a separate detail snapshot.
The default Information menu groups the IP into Wi-Fi instead of a separate row.
The ip binding remains available for other compositions.

nabla_device_model sets a model label (maximum 64 bytes); the JC3248W535CN entry
point supplies its reference. Without Wi-Fi support, network values are unavailable.
With Wi-Fi, connected SSID and first IP address are read from ESPHome once per
second and cleared on disconnection. No passwords, server URLs or credentials
are displayed or logged. Version is the running ESPHome build version.
Uptime currently uses millis and wraps after approximately 49.7 days.

Model details show the build chip variant (Host / SDL on the simulator),
nabla_display_kind, configured screen_width/screen_height and
nabla_display_monochrome. Declare display technology explicitly: LCD, OLED,
e-paper, etc. Monochrome does not imply OLED. Resolution is the configured
canvas, not an orientation-dependent sensor reading.
The JC3248W535CN composition declares IPS LCD; its chip comes from ESPHome.

Wi-Fi summaries combine SSID and IP. Details add gateway, netmask and RSSI.
Gateway/netmask use the ESP32 station interface, including DHCP values.
Unsupported or disconnected fields show the localized unavailable value;
each refresh clears old network details. No router brand/model is inferred.

The snapshot holds five summary strings and two detail strings, not network clients. Existing menu
focus, Up/Down/Enter/Back, touch, scrolling and themes apply. Regular labels update
without rebuilding navigation or changing focus. Compact rows retain marquee
behavior. Long/nonbundled SSID characters remain limited by the selected fonts.

The launcher no longer runs simulated boot progress or the logo spin on startup.
toolbar_set_progress and the logo animation helpers remain available for actual
operations. This removes the deliberate delay; hardware/network initialization
still takes its normal time.

Run ./simulator/run.sh information for an explicitly synthetic host fixture.
It uses documentation-only addresses and demo labels; nabla_info_preview defaults
to false and is ignored outside USE_HOST. This iteration was checked on the host,
not uploaded to hardware. Real gateway display still needs target verification.
