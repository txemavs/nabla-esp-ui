# Headless web renderer (preview)

Nabla navigation without a physical display. The ESP32 embeds the complete HTML
page and uses the device-owned nabla_navigation tree. No CDN or Home Assistant
is needed. Open / or /nabla on the device HTTP server (port 80).

Dependencies: ESP32, Wi-Fi, nabla_navigation, nabla_wifi_compact, nabla_runtime
(export components/forms/wifi_flow.h), web_server_base and json.
Set camera_node to a leaf key in the tree and camera_port to the existing
MJPEG server port. Camera transport remains owned by esp32_camera_web_server.
Do not also install web_server: it competes for the root URL.

The initial Spanish browser view provides camera fullscreen, navigation,
information details, a browser-local light/dark switch, network scanning,
masked password entry, explicit save confirmation and cancel.
At the root, the shared triangle switches tiles/list and remembers the view in browser local storage. Inside a section it returns to the parent. Nested menus use compact rows. The logo has a one-second depth turn with reduced-motion support. Tab/Enter, Up/Down, Home/End and Escape provide keyboard navigation; returning restores the launching item. Text fields retain native editing keys.
Only open, information and wifi nodes are supported; other actions are rejected.
This is a new renderer, not a bitmap mirror of the physical framebuffer.

Wi-Fi operations reuse WifiFlow and its real adapter. Fallback networks are
preserved. Draft passwords are not returned in JSON or stored in browser storage.
POST operations require a per-boot same-origin token. This is CSRF protection,
not user authentication: the page is for trusted LAN/protected fallback AP use.
The existing camera stream keeps its existing access controls.
Changing Wi-Fi may disconnect the browser; reconnect using the new IP or AP.

The HTTP callbacks queue a single bounded command; the main loop performs
radio operations. Status is cached once per second and polled every two seconds.
Scans expose at most eight results. A single shared Wi-Fi operation exists per
device; multiple browsers observe that shared state.
The captive portal still handles OS detection URLs; the menu handler registers
first so its root and API remain reachable in AP mode.

Verified: ESPHome 2026.8.2 ESP32-S3 Arduino compilation, OTA, live HTTP page/state,
and real network scan. Password migration, AP recovery and browser visual
qualification remain to be tested. Existing camera image quality is independent.
No new camera sensor settings are introduced.

page.html is the source; run python3 tools/embed_web_page.py after editing it.

## Next phase (not implemented)

A display mirror should expose the physical display pixels and forward input to
its existing controller, while this responsive renderer retains independent
browser navigation. First qualify the small monochrome framebuffer; do not
claim universal display capture before checking each driver.

A later visual menu editor could reuse the validated navigation schema and
preview a single tree in several device profiles. Export device-owned YAML;
keep credentials and installation data out of shared examples. Browser preview,
pixel mirroring and YAML editing are separate capabilities.
