# Display mirror HTTP contract

Version: Phase 1 (2026-09-20). Related: [issue #34](https://github.com/txemavs/nabla-esp-ui/issues/34).

The display mirror exposes logical framebuffer contents over HTTP for remote
viewing and optional encoder input. Home Assistant or other consumers convert
frames off-device; the ESP serves raw bytes without JPEG encoding.

## Reference profiles

| Profile | Dimensions | Format | Frame size | Input |
|---------|------------|--------|------------|-------|
| Kit1 (ST7735) | 160×128 | `rgb332` | 20,480 bytes | encoder |
| T-Call (SSD1309) | 128×64 | `mono1` | 1,024 bytes | encoder |

These are the two verified hardware configurations used to freeze this contract.
Other sizes (LVGL 240×240, 480×320) follow the same format rules.

## Endpoints

All endpoints are relative to the device HTTP root (e.g. `http://<device-ip>/`).

### `GET /mirror/capabilities`

Returns JSON describing the display and input configuration.

**Response**: `200 OK`, `Content-Type: application/json`

```json
{
  "width": 160,
  "height": 128,
  "format": "rgb332",
  "input": true
}
```

| Field | Type | Description |
|-------|------|-------------|
| `width` | integer | Logical display width in pixels |
| `height` | integer | Logical display height in pixels |
| `format` | string | Pixel format: `"mono1"` or `"rgb332"` |
| `input` | boolean | Whether encoder input is accepted |

`touch` is an optional boolean (absent means false), independent of `input`.
It advertises short LVGL taps, enabled explicitly with `touch: true`. Consumers
must not infer touch support from screen size. Ignore unknown fields.

### `GET /mirror/frame`

Returns the raw framebuffer contents.

**Response**: `200 OK`, `Content-Type: application/octet-stream`  
**Headers**: `Cache-Control: no-store`

**Error responses**:
- `409 Conflict`: Frame not yet ready
- `503 Service Unavailable`: Rate-limited (minimum 500ms between requests) or
  another frame transfer is in progress

Frame requests are rate-limited and serialized to protect HTTP stack stability
under concurrent ESPHome native API connections and mirror polling. Clients
should implement backoff on 503 responses rather than tight retry loops.

#### Byte layout

**`mono1` (monochrome)**:
- Size: `ceil(width * height / 8)` bytes
- Bit order: MSB first, row-major
- Pixel 0 is bit 7 of byte 0; pixel 7 is bit 0 of byte 0
- White pixel = bit set (1); black pixel = bit clear (0)

For a 128×64 display: 128 × 64 / 8 = 1,024 bytes.

```
Byte 0: [px0 px1 px2 px3 px4 px5 px6 px7]  (MSB to LSB)
Byte 1: [px8 px9 px10 px11 px12 px13 px14 px15]
...
```

**`rgb332` (8-bit color)**:
- Size: `width * height` bytes (one byte per pixel)
- Byte layout: `RRRGGGBB` (3 bits red, 3 bits green, 2 bits blue)
- Row-major order, top-left to bottom-right

For a 160×128 display: 160 × 128 = 20,480 bytes.

```
bits[7:5] = red   (0-7 → 0-255 as r * 255 / 7)
bits[4:2] = green (0-7 → 0-255 as g * 255 / 7)
bits[1:0] = blue  (0-3 → 0-255 as b * 255 / 3)
```

### `GET /mirror/token`

Returns a per-boot CSRF token required for input actions.

**Response**: `200 OK`, `Content-Type: text/plain`  
**Headers**: `Cache-Control: no-store`

The token is a 16-character hexadecimal string, regenerated on each device boot.

### `POST /mirror/action`

Sends an encoder/button action to the device controller.

**Request**:
- Header: `X-Nabla-Token: <token>` (from `/mirror/token`)
- Body: `action=<value>` (form-urlencoded)

**Allowed actions**: `up`, `down`, `enter`, `back`

**Response**:
- `200 OK`: Action queued
- `400 Bad Request`: Missing or invalid action
- `401 Unauthorized`: Missing or invalid token
- `404 Not Found`: Input disabled (no `on_action` configured)
- `409 Conflict`: Previous action still pending

Only one action can be pending at a time. The main loop processes it and clears
the queue before another can be accepted.

### `POST /mirror/touch`

Optional LVGL short tap. Requires `touch: true` and `lvgl_id` in YAML.
Send form fields `x` and `y` (non-negative decimal integers) with the same
`X-Nabla-Token` header. Coordinates are logical framebuffer pixels:
`0 <= x < width`, `0 <= y < height`. Do not apply physical touchscreen
rotation/transforms again. Viewers must remove image borders/letterboxing
and account for CSS scaling before converting to integer coordinates.

Responses: 200 queued, 400 invalid coordinates, 401 invalid token, 409 busy
or display not ready. Unsupported firmware does not advertise the capability.
A queued tap can be cancelled by local physical touch or paused LVGL.

Only one remote tap can be pending/pressed. LVGL emits a press for 80ms then
releases it locally, even if the client disconnects. A tap waiting over 500ms
before its first read expires. HTTP callbacks never call LVGL. Runtime changes
that alter mirror dimensions disable admission until geometry matches again;
the existing mirror runtime-rotation qualification limits still apply.

No drag/held-input protocol is exposed yet. Do not retry a timeout: an action
may already have executed. A 401 may be retried after fetching a fresh token.

### `GET /` and `GET /mirror`

Returns an HTML viewer page with canvas rendering and optional controls.
Available when the mirror component owns the root (standalone mode) or when
coordinated through `nabla_web_service`.

## Security

- **LAN only**: Do not expose the mirror to the public Internet.
- **Token protection**: Input requires a per-boot token; this is CSRF protection,
  not user authentication. Anyone on the LAN can obtain the token.
- **No secrets in repo**: Device YAML with real credentials stays in private
  installations, never in this repository.

## Conversion examples

**rgb332 → RGB888** (Python):
```python
def rgb332_to_rgb888(byte):
    r = ((byte >> 5) & 0x07) * 255 // 7
    g = ((byte >> 2) & 0x07) * 255 // 7
    b = (byte & 0x03) * 255 // 3
    return (r, g, b)
```

**mono1 → pixels** (Python):
```python
def mono1_to_pixels(data, width, height):
    pixels = []
    for i in range(width * height):
        byte_idx = i // 8
        bit_idx = 7 - (i % 8)
        pixels.append(255 if (data[byte_idx] >> bit_idx) & 1 else 0)
    return pixels
```

## Frame rates and bandwidth

| Format | Frame size | ~10 FPS | Typical |
|--------|------------|---------|---------|
| mono1 128×64 | 1 KB | 10 KB/s | 10 FPS polling |
| rgb332 160×128 | 20 KB | 200 KB/s | ~2-3 FPS polling |
| rgb332 240×240 | 56 KB | 560 KB/s | ~1-2 FPS |
| rgb332 480×320 | 150 KB | 1.5 MB/s | <1 FPS |

The browser viewer polls sequentially; actual FPS depends on network latency.
Hidden tabs pause polling. Multiple viewers share the same captured frame.

## Future extensions (not implemented)

- **Touch input**: Absolute coordinates via `POST /mirror/touch`
- **rgb565**: 16-bit color for higher fidelity (doubles bandwidth)
- **Dirty rectangles**: Partial updates to reduce bandwidth
- **WebSocket**: Push frames instead of polling

These are mentioned in issue #34 as later phases. This contract covers only
what is implemented and verified today.

## Component integration

See the [nabla_display_mirror component](../../external_components/nabla_display_mirror/README.md)
for ESPHome configuration. The component README covers YAML setup, memory usage
and physical evidence. This document specifies the HTTP contract for consumers.

## Optional MQTT address discovery

[`nabla_presence`](../../external_components/nabla_presence/README.md) adds an
independent `/nabla/identity` endpoint and MQTT identity/address hints. It does not
change mirror frames, capabilities, tokens or input actions. Consumers retain the
existing device identity while updating only a verified address. It is opt-in on
both firmware and Nabla Control; availability still comes from HTTP polling.
