# Monochrome display mirror (experimental)

An offline HTML canvas displays the same logical 128x64 monochrome pixels sent
to the physical display. This is a draw-pass tee, not a second UI renderer or
direct access to a driver's private framebuffer. No Home Assistant is required.

Only ESPHome Display API monochrome 128x64 compositions are qualified here.
LVGL/color displays are unsupported. A nonmatching display renders normally but
does not publish frames. Hardware inversion, power and controller effects after
the draw pass are not represented. The firmware must use the wrapper for every
draw pass to mirror it.

Dependencies: ESP32, Wi-Fi, display and web_server_base. Standalone mode owns / and /mirror. With nabla_web_service it owns only /mirror
and its API; the coordinator can combine both views. ESPHome web_server remains
incompatible. Omit on_action for a read-only mirror.

## Integration

Enable the external component and forward only the allowed navigation actions:

    nabla_display_mirror:
      id: screen_mirror
      on_action:
        - lambda: |-
            auto &shell=nabla::compact_shell;
            if(action=="up")shell.move(-1);
            else if(action=="down")shell.move(1);
            else if(action=="enter")shell.activate();
            else if(action=="back")shell.back();

In the existing display lambda, pass the returned surface to the existing
renderer instead of it. Keep the same fonts, configuration and real display:

    auto &surface=id(screen_mirror).begin(it);
    // existing shell.render(surface, ...);
    id(screen_mirror).end();

Do not draw twice. The underlying display still handles physical rotation.
The logical capture remains upright. Existing encoder input is unchanged.

## Browser and transport

Open /mirror. The canvas uses nearest-neighbor pixel scaling. Up/Down,
Enter/Escape and four buttons forward navigation to the same controller as the
encoder. There is no browser-local selection.

GET /mirror/frame returns exactly 1024 bytes, row-major, MSB first, one bit per
pixel. Two 1024-byte buffers separate rendering and network access. The page
polls sequentially at up to 10 FPS and pauses while hidden. It never queues
overlapping frame requests. Wi-Fi delay reduces the effective frame rate.

GET /mirror/token returns a per-boot token. POST /mirror/action accepts only
up/down/enter/back with X-Nabla-Token. A single bounded pending action is
processed on the main loop; competing requests receive 409. Tokens prevent
cross-origin form submissions, not access by other trusted LAN/AP clients.
The initial HTTP viewer is not intended for unauthenticated Internet exposure.
Do not share the endpoint publicly; screen contents can contain private data.

## Evidence and limits

ESPHome 2026.8.2, original ESP32 + SSD1309/ssd1306_spi 128x64, ESP-IDF:
compiled, uploaded over OTA, received and inspected a real 1024-byte frame.
A remote Down changed the pixels; Up was sent to restore selection. Invalid
tokens received 401. No load-control action or credential edit was exercised.
Physical/browser simultaneous operation and long-duration heap testing still
need owner confirmation. Audio functionality remains unqualified.

The tested composition's build reported 99,516 bytes static RAM and 1,526,255
bytes application flash. These are build figures, not available runtime heap.
A future menu editor should reuse navigation schema/profile selection; this
mirror only displays and controls an existing device.
