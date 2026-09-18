# nabla_ha

Optional Home Assistant REST API client for ESP32 panels. Polls entity states
and sends toggle/brightness commands over HTTPS without requiring the native
ESPHome API or WebSocket connection.

[Back to external_components](../README.md)

## What it does

- Maintains a background FreeRTOS task for HTTP requests.
- Polls configured `light.*` and `switch.*` entities in round-robin.
- Tracks on/off state, availability and brightness (for dimmable lights).
- Sends `turn_on` / `turn_off` commands with optional brightness.
- Uses TLS certificate verification and persistent HTTP connections.
- Exposes a dirty flag for UI refresh without polling from the main loop.

## Relationship to adapters

This component is a standalone REST client. It does not use the ESPHome `api:`
integration or Home Assistant's WebSocket API. Panels can use it alongside or
instead of MQTT adapters when direct HA control is preferred.

## Configuration

```yaml
nabla_ha:
  id: ha_client
  url: !secret ha_url
  token: !secret ha_token
  entities:
    - light.living_room
    - switch.garage_door
```

### Schema

| Key | Required | Description |
|-----|----------|-------------|
| `id` | no | Component ID for referencing in lambdas |
| `url` | yes | Home Assistant HTTPS origin (e.g. `https://ha.local:8123`) |
| `token` | yes | Long-lived access token |
| `entities` | yes | List of 1–8 `light.*` or `switch.*` entity IDs |

### URL validation

The `url` must be an HTTPS origin without path, query or embedded credentials:

```
https://ha.local:8123      ✓
https://192.168.1.100      ✓
http://ha.local            ✗ (not HTTPS)
https://user:pass@ha.local ✗ (credentials in URL)
https://ha.local/api       ✗ (path not allowed)
```

### Entity validation

Only explicitly listed `light.*` and `switch.*` entity IDs are accepted.
Arbitrary entity patterns or wildcards are not supported.

## Runtime API

```cpp
// Check connection status
if (ha_client->ready()) { ... }

// Get entity state (returns State{} if index invalid)
auto state = ha_client->state(0);
bool is_on = state.on;
bool can_dim = state.dimmable;
int brightness_pct = state.brightness;  // 0–100

// Check if entity is available and fresh
if (ha_client->available(0)) {
  // Entity responded recently, not pending a command
}

// Send a command (returns false if unavailable or busy)
ha_client->command(0, true, 80);  // turn on at 80% brightness

// Check if state changed since last read
if (ha_client->consume_dirty()) {
  // Refresh UI
}
```

### Status values

| Status | Meaning |
|--------|---------|
| `CONNECTING` | Initial state, first poll pending |
| `READY` | Successfully communicating with HA |
| `UNREACHABLE` | Network error or Wi-Fi disconnected |
| `UNAUTHORIZED` | Token rejected (401/403) |
| `INVALID` | Unexpected response format |

## Dependencies and limits

- Requires ESP-IDF (ESP32) with Wi-Fi.
- Auto-loads `json` component for response parsing.
- Enables full mbedTLS certificate bundle for HTTPS validation.
- Maximum 8 entities per client instance.
- Entity state considered stale after 30 seconds without update.
- Poll interval: 600 ms when ready, 8 s when reconnecting.
- HTTP timeout: 3 s per request.

## Security notes

- Never include tokens in public YAML; always use `!secret`.
- The component logs the HA URL but not the token.
- Commands are fire-and-forget; the next poll confirms the result.
- Failed POST requests are not retried to avoid duplicate actions.
