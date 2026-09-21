# Local equipment gallery

Open index.html through a local static HTTP server. Select OLED 128x64,
ST7735 160x128, T-Watch 240x240 or the 480x320 panel and enter its URL.
Addresses are stored only in that browser. No device discovery or cloud backend.
The iframe uses the device's own viewer; switching targets does not reconfigure
hardware. HTTPS pages may need the direct HTTP link instead of embedding.

This is an initial live-view selector, NOT a cross-profile menu simulator or
editor. No sample private addresses are committed. Offline/unreachable devices
require reconnection; the gallery cannot reliably inspect cross-origin status.
T-Watch is a supported capture build target, with live verification recorded
separately from profile registration.

Local launcher integrations may seed addresses with a JSON setup parameter in
the URL fragment. It is consumed locally and removed from the address bar.
Never include credentials in these URLs.
