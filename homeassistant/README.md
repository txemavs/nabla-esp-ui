# Nabla Display for Home Assistant

MVP Home Assistant integration for viewing Nabla ESP-UI display mirrors and
sending encoder actions. Phase 2 of [issue #34](https://github.com/txemavs/nabla-esp-ui/issues/34).

## Overview

This integration polls ESP devices running the `nabla_display_mirror` component
and converts their raw framebuffer data (RGB332 or mono1) into viewable images.
For devices with encoder input, it exposes buttons and a Lovelace card remote.

**Reference devices:**
- Kit1-class (ST7735): 160×128 pixels, RGB332 (8-bit color)
- T-Call-class (SSD1309): 128×64 pixels, mono1 (monochrome)

See [DISPLAY-MIRROR-CONTRACT.md](../docs/platform/DISPLAY-MIRROR-CONTRACT.md)
for the HTTP contract specification.

## Installation

### Custom component

1. Copy `custom_components/nabla_display/` to your Home Assistant config:
   ```
   /config/custom_components/nabla_display/
   ```

2. Add configuration to `configuration.yaml`:
   ```yaml
   nabla_display:
     devices:
       - host: "DEVICE_IP"
         name: "Kit1 Display"
         poll_interval: 1.0
       - host: "DEVICE_IP_2"
         name: "T-Call Display"
         poll_interval: 0.5
   ```

3. Restart Home Assistant.

### Lovelace card

1. Copy `www/nabla-display-card.js` to:
   ```
   /config/www/nabla-display-card.js
   ```

2. Add the resource in Lovelace configuration (Settings → Dashboards → Resources):
   ```yaml
   url: /local/nabla-display-card.js
   type: module
   ```

3. Add a card to your dashboard:
   ```yaml
   type: custom:nabla-display-card
   device_id: "192_168_1_100"  # Host IP with dots replaced by underscores
   name: "Kit1 Display"
   poll_interval: 1000  # Milliseconds
   scale: 2             # Display scaling factor
   show_controls: true  # Show encoder buttons
   ```

## Configuration options

### Integration (configuration.yaml)

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `host` | string | required | Device IP or hostname |
| `name` | string | "Nabla Display {host}" | Friendly name |
| `poll_interval` | float | 1.0 | Seconds between frame fetches (0.2-30) |

### Lovelace card

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `device_id` | string | required | Device ID (host with dots as underscores) |
| `name` | string | "Nabla Display" | Card title |
| `poll_interval` | number | 1000 | Milliseconds between frame fetches |
| `scale` | number | 2 | Display scale factor (1-5) |
| `show_controls` | boolean | true | Show encoder buttons |

## Services

### nabla_display.send_action

Send an encoder action to a device.

| Parameter | Type | Description |
|-----------|------|-------------|
| `device_id` | string | Device ID |
| `action` | string | One of: `up`, `down`, `enter`, `back` |

Example service call:
```yaml
service: nabla_display.send_action
data:
  device_id: "192_168_1_100"
  action: "enter"
```

## Entities

For each device with input enabled, four button entities are created:
- `button.nabla_display_{device_id}_up`
- `button.nabla_display_{device_id}_down`
- `button.nabla_display_{device_id}_enter`
- `button.nabla_display_{device_id}_back`

Read-only devices (no input capability) do not get button entities.

## HTTP API

The integration exposes a frame image endpoint:

```
GET /api/nabla_display/{device_id}/frame
```

Response headers include display metadata:
- `X-Nabla-Width`: Display width
- `X-Nabla-Height`: Display height
- `X-Nabla-Format`: Pixel format (rgb332/mono1)
- `X-Nabla-Input`: Input capability (1/0)

## Architecture

Frame decoding happens server-side in Python (PIL):
- RGB332 bytes are expanded to RGB888 and encoded as PNG
- mono1 bits are expanded to grayscale and encoded as PNG

This keeps the Lovelace card simple and works with the mobile app (which can
fetch the frame image directly). The card just displays the PNG and sends
encoder actions via HA services.

## Security notes

- Uses the device's per-boot CSRF token for input actions
- LAN-only: do not expose devices to the public Internet
- Device IP is used as the device ID; no discovery/mDNS in MVP

## Limitations (MVP)

- No config flow (YAML-only configuration)
- No device registry integration
- No touch coordinate support (later phase)
- No HACS manifest (manual installation)
- No WebSocket push (polling only)

## Testing notes

### Kit1 (160×128 rgb332)

Frame size: 20,480 bytes. At 1 FPS polling, bandwidth is ~20 KB/s.
Encoder actions: up/down/enter/back via X-Nabla-Token.

### T-Call (128×64 mono1)

Frame size: 1,024 bytes. At 10 FPS polling, bandwidth is ~10 KB/s.
Same encoder actions via X-Nabla-Token.

## Development

Run frame decode tests from the repository root:
```bash
python3 -m unittest tests.test_nabla_display -v
```
