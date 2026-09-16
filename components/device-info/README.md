# Device information

package.yaml registers a read-only runtime snapshot. The shared catalog includes
it; use info: model, wifi, ip, version or uptime on an ordinary leaf (no action,
static detail or children). Information is the first Settings child in the demo.
Both regular and compact menu rows use the snapshot; a leaf opens its full value.

nabla_device_model sets a model label (maximum 64 bytes); the JC3248W535CN entry
point supplies its reference. Without Wi-Fi support, network values are unavailable.
With Wi-Fi, connected SSID and first IP address are read from ESPHome once per
second and cleared on disconnection. No passwords, server URLs or credentials
are displayed or logged. Version is the running ESPHome build version.
Uptime currently uses millis and wraps after approximately 49.7 days.

The snapshot holds five bounded strings, not network clients. Existing menu
focus, Up/Down/Enter/Back, touch, scrolling and themes apply. Regular labels update
without rebuilding navigation or changing focus. Compact rows retain marquee
behavior. Long/nonbundled SSID characters remain limited by the selected fonts.

The launcher no longer runs simulated boot progress or the logo spin on startup.
toolbar_set_progress and the logo animation helpers remain available for actual
operations. This removes the deliberate delay; hardware/network initialization
still takes its normal time.
