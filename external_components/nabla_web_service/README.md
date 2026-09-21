# Optional web service coordinator

This component gives responsive and mirror views one root route. Include the
renderers you need and choose mode: responsive, mirror or both. Omit the web
components entirely to build without them. This initial schema deliberately
has no enabled:false field: omission is the compile-time off switch.

~~~yaml
nabla_web_service:
  mode: both
nabla_web:
  camera_node: camera
nabla_display_mirror:
  id: screen_mirror
~~~

This example fragment also requires navigation, the real Wi-Fi adapter, runtime
headers and a compatible display render hook. See examples/web-service/both.yaml
for a compile-only complete fixture. It does not supply a camera stream server.

Root redirects to /nabla or /mirror, or shows a two-view chooser. Both renderers
must be present for both mode; mismatched configuration fails validation.
Old standalone renderer configurations retain their original root behavior.
ESPHome web_server cannot also own the root. Captive portal remains available.

A mirror with no on_action is read-only: its handler does not accept control
requests and the browser hides the controls. Adding on_action explicitly enables
the four allowlisted inputs. This initial implementation disables routing at
runtime; code-size elimination of the input machinery remains pending.

The responsive renderer still supports only open, Wi-Fi and information nodes.
Do not enable it on a command-rich installation until those actions have a
supported adapter. Read-only mirroring can display any already-rendered content.

The four studio examples are OLED 128x64, ST7735 160x128, T-Watch 2020 240x240
and the large touch panel 480x320. Monochrome and RGB332 capture are implemented; see the mirror component for evidence.
The watch is a first-class gallery target, not a scaled OLED preview.

Auth integration, AP recovery, resource/latency benchmarks and extended color
mirror qualification remain open W1/W2 gates. Per-boot tokens only protect against cross-origin form
submissions; use trusted LAN or protected AP access. No Internet access claim.
