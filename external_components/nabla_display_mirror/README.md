# Display mirror (experimental)

An offline canvas receives logical pixels from the actual device drawing path.
Compact Display API compositions use begin/end around their existing render
call. LVGL compositions wrap the ESPHome flush callback and assemble partial
rectangles, publishing when the last area is flushed. Neither draws the UI twice.

## Configuration

The original 128x64 monochrome defaults remain compatible. Color capture uses
RGB332 (256 colors) to limit RAM and bandwidth; geometry is preserved but colors
are quantized from the physical display. Width/height are logical dimensions.

~~~yaml
nabla_display_mirror:
  id: screen_mirror
  width: 160
  height: 128
  color: true
~~~

For compact displays, keep the existing render call but replace its display:

    auto &surface=id(screen_mirror).begin(it);
    // existing shell.render(surface, ...);
    id(screen_mirror).end();

For LVGL use lvgl_id instead of the display lambda wrapper:

~~~yaml
nabla_display_mirror:
  id: screen_mirror
  width: 240
  height: 240
  color: true
  lvgl_id: nabla_lvgl
~~~

Tested configuration sizes: OLED 128x64, ST7735 160x128, LVGL 240x240 and
480x320. The LVGL hook is specific to ESPHome's RGB565 flush implementation.
It forwards every flush to LvglComponent::static_flush_cb, retaining the
original display user data. Runtime rotation changes are not qualified:
configure dimensions/orientation to match the active LVGL screen at startup.

No on_action means read-only. Optional on_action receives up/down/enter/back;
map them to the existing controller. HTTP callbacks queue one pending action,
executed by the main loop. No generic LVGL touch injection is implemented.
Pixel geometry remains synchronized with physical input.

## Memory and transport

Two persistent frame buffers use RAMAllocator (PSRAM preferred, internal RAM
fallback). An HTTP request allocates one temporary frame copy. Allocation
failure returns an unavailable view rather than modifying the display.
A monochrome 128x64 frame is 1024 bytes; RGB332 frames are 20,480 / 57,600 /
153,600 bytes for the three color sizes. Measure heap and input latency before
raising frame rates or adding viewers; no prolonged load qualification yet.

GET /mirror/capabilities describes dimensions, format and input availability.
GET /mirror/frame returns row-major mono1 (MSB first) or RGB332 bytes.
The browser polls sequentially at up to 10 FPS monochrome or about 2.8 FPS color,
excluding network latency. Hidden pages pause and disconnected pages retry.
Snapshots are shared across viewers; capture currently still runs without them.

Input uses a per-boot X-Nabla-Token, not user authentication. Use a trusted LAN
or protected AP; do not expose private display contents on the public Internet.
Standalone mode owns / and /mirror. nabla_web_service coordinates combined
responsive/mirror roots. ESPHome web_server remains incompatible.

## Evidence

- OLED: physical capture inspected; remote Down changed the frame and Up restored
  selection; invalid token returned 401.
- Kit1: USB write verified; real 160x128 frame received and visually inspected.
- Large LVGL panel: OTA successful; real 480x320 color frame received and inspected.
- T-Watch: 240x240 firmware compiled; physical verification pending device power.
- Browser profile switching is a live equipment gallery, not a menu simulator.

Hardware inversion/backlight effects after drawing are not mirrored. RGB332
quantization, runtime rotations, overlapping clients, audio coexistence and
long-duration heap behavior remain explicit qualification limits.
