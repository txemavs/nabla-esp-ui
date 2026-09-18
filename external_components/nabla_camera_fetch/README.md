# nabla_camera_fetch

Bounded background HTTPS JPEG fetcher for ESP32 regular displays. Fetches camera
snapshot images over HTTPS and decodes them into ESPHome `runtime_image` slots
without blocking the main loop.

[Back to external_components](../README.md)

## What it does

- Runs a dedicated FreeRTOS task for network I/O.
- Fetches HTTPS JPEG images with TLS certificate verification.
- Decodes into one or two `runtime_image` slots for LVGL display.
- Enforces a 128 KB response limit and 6 s total timeout.
- Uses PSRAM for downloaded bytes; decoding happens on the main loop.
- Reuses HTTP connections when the server supports keep-alive.

## When to use it

Use this component when your panel displays camera snapshots from a Home
Assistant instance or another HTTPS endpoint that returns JPEG images. It
handles the network fetch and decode cycle asynchronously.

Do not use it for video streams, non-JPEG formats, or endpoints requiring
complex authentication beyond a bearer token in the URL.

## Configuration

```yaml
runtime_image:
  - id: camera_slot_0
    type: RGB565
    buffer_size: 65536

nabla_camera_fetch:
  id: fetcher
  images:
    - camera_slot_0
  on_result:
    - lambda: |-
        ESP_LOGI("cam", "slot=%d success=%d", slot, success);
```

### Schema

| Key | Required | Description |
|-----|----------|-------------|
| `id` | no | Component ID for referencing in lambdas |
| `images` | yes | List of 1–2 `runtime_image` IDs to decode into |
| `on_result` | no | Automation triggered with `slot` (int) and `success` (bool) |

## Runtime API

```cpp
// Select which slot subsequent requests target
fetcher->select(0);

// Request a fetch (returns false if busy, URL invalid, or Wi-Fi disconnected)
bool started = fetcher->request(0, "https://ha.local/api/camera_proxy/camera.front?token=...");
```

The `on_result` trigger fires after each fetch completes, reporting the slot
index and whether decoding succeeded.

## Dependencies and limits

- Requires ESP-IDF (ESP32) with Wi-Fi.
- Auto-loads `runtime_image` component.
- Enables full mbedTLS certificate bundle for HTTPS validation.
- Maximum response size: 128 KB.
- Network timeout: 4 s connect, 6 s total.
- URL must start with `https://` and fit in 1024 bytes.
- Only one fetch runs at a time; requests while busy return false.

## Notes

- The component does not store or manage camera URLs; the caller provides them.
- Authentication tokens belong in the URL query string, not in component config.
- Keep credentials out of public YAML; use `!secret` or runtime injection.
