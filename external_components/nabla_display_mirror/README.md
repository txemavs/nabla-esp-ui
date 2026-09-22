# Display mirror (experimental)

An offline canvas receives logical pixels from the actual device drawing path.
See [DISPLAY-MIRROR-CONTRACT.md](../../docs/platform/DISPLAY-MIRROR-CONTRACT.md)
for the complete HTTP endpoint specification and byte-layout documentation.

Compact Display API compositions use begin/end around their existing render
call. LVGL compositions wrap the ESPHome flush callback and assemble partial
rectangles, publishing when the last area is flushed. Neither draws the UI twice.

## ESPHome native API incompatibility (large panels)

**On large-panel devices (e.g. JC3248W535CN 480×320), HTTP frame serving and
ESPHome native API (`api:`) are mutually exclusive.** Enabling both causes HTTP
stack failure within seconds, regardless of frame size or polling rate.

Field-tested failure modes (2026-09-22):
- Chunked HTTP transfer with task yields — failed
- Rate-limited sends (500ms interval) — failed  
- Downscaled preview (9.6KB instead of 153KB) — failed

Root cause: The ESP-IDF HTTP server and native API share TCP resources. Any
HTTP frame transfer — even small previews — starves the stack when API is active.

### Supported configurations

| Mode | `api:` | Mirror | Use case |
|------|--------|--------|----------|
| **Mirror + MQTT** | disabled/omitted | frame polling OK | HA screen preview via Nabla Control |
| **API + presence** | enabled | presence-only (no frame) | HA entity control, no screen preview |

### Supported HA path for large panels

The validated configuration for Panel 480 (JC3248W535CN @ 10.10.10.40):

1. **ESPHome native API: OFF** — disable or omit the HA config entry
2. **Entity control: MQTT only** — lights via `nabla/control` topics
3. **Nabla Control: HTTP mirror ON** — preview mode (120×80), no `?full=1`

```yaml
# Panel YAML — no api: section
mqtt:
  broker: !secret mqtt_broker
  topic_prefix: nabla/control/panel

nabla_display_mirror:
  id: screen_mirror
  width: 480
  height: 320
  color: true
  lvgl_id: nabla_lvgl
```

In Home Assistant, keep the ESPHome config entry for this device **disabled**
(not deleted — disabled). Use Nabla Control custom component for mirror preview
and MQTT for entity state/commands.

### Alternative: API without screen preview

If native API is required, configure Nabla Control for **presence-only** mode
(disable `/mirror/frame` polling). Screen preview is unavailable in this mode.

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
  touch: true  # Optional short remote taps; defaults to false.
~~~

Tested configuration sizes: OLED 128x64, ST7735 160x128, LVGL 240x240 and
480x320. The LVGL hook is specific to ESPHome's RGB565 flush implementation.
It forwards every flush to LvglComponent::static_flush_cb, retaining the
original display user data. Runtime rotation changes are not qualified:
configure dimensions/orientation to match the active LVGL screen at startup.

Without `on_action` and `touch`, the mirror is read-only. Optional on_action receives up/down/enter/back;
map them to the existing controller. HTTP callbacks queue one pending action,
executed by the main loop. Optional `touch: true` requires `lvgl_id` and registers
a separate LVGL pointer. HTTP only queues coordinates; the LVGL input callback
produces an 80ms press/release. Physical touch takes priority. Drags and long
presses are not supported in this phase.
Pixel geometry remains synchronized with physical input.

## Memory and transport

Four persistent frame buffers use RAMAllocator (PSRAM preferred, internal RAM
fallback): drawing surface, published snapshot, send buffer, and optional
preview buffer. A monochrome 128x64 frame is 1024 bytes; RGB332 frames are
20,480 / 57,600 / 153,600 bytes for the three color sizes.

### Downscaled preview for large panels

Panels with frames >32KB (e.g. 480×320 = 153KB) automatically serve a
downscaled preview by default. This is critical for HTTP stability when
ESPHome native API is active concurrently.

| Panel | Full frame | Preview | Scale |
|-------|------------|---------|-------|
| 480×320 | 153,600 bytes | 120×80 = 9,600 bytes | 4× |
| 240×240 | 57,600 bytes | 120×120 = 14,400 bytes | 2× |
| 160×128 | 20,480 bytes | (no preview needed) | 1× |
| 128×64 mono | 1,024 bytes | (no preview needed) | 1× |

### Endpoints

GET /mirror/capabilities returns width/height/format plus preview_width,
preview_height, preview_scale when a preview is available.

GET /mirror/frame returns the preview by default for large panels.
Add `?full=1` to request full resolution (may cause HTTP issues under API load).
Response header `X-Nabla-Preview: WxH` indicates preview dimensions.

Frame requests are rate-limited to 100ms minimum interval and serialized.
Additional requests receive 503 Service Unavailable. Clients should implement
backoff rather than tight retry loops.

Input uses a per-boot X-Nabla-Token, not user authentication. Use a trusted LAN
or protected AP; do not expose private display contents on the public Internet.
Standalone mode owns / and /mirror. nabla_web_service coordinates combined
responsive/mirror roots. ESPHome web_server remains incompatible.

## Evidence

- OLED: physical capture inspected; remote Down changed the frame and Up restored
  selection; invalid token returned 401.
- Kit1: USB write verified; real 160x128 frame received and visually inspected.
- Large LVGL panel: OTA successful; real 480x320 color frame received and inspected.
- T-Watch: OTA succeeded on 2026-09-21 using mirror revision 0429cdf; real
  240x240 RGB332 frame (57,600 bytes) received and visually inspected.
  The original build reported input=false. On 2026-09-21 the touch build was
  compiled and uploaded; touch=true and a remote tap opened Settings, with
  a second tap on the triangle returning to the menu. Encoder input remains false.
- Browser profile switching is a live equipment gallery, not a menu simulator.

Hardware inversion/backlight effects after drawing are not mirrored. RGB332
quantization, runtime rotations, overlapping clients, audio coexistence and
long-duration heap behavior remain explicit qualification limits.
