# Nabla Net MQTT protocol

This document describes the MQTT topic structure and message format used by
Nabla panels to control Home Assistant entities across multiple sites without
native API connections. A single broker serves panels at different locations.

The protocol is implemented by `adapters/mqtt-lights/`. This document provides
the installation context; see that adapter's README for code-level details.

## Design principles

1. **Local first**: panel menu and navigation work without broker connectivity.
2. **Authoritative state from the server**: panels receive state; they do not invent it.
3. **Retained state, non-retained commands**: state topics persist; commands do not.
4. **Freshness validation**: panels reject stale or future-dated state messages.
5. **Allowlisted topics**: the broker (and any bridges) only forward configured mappings.
6. **Private bindings**: real topics, entity IDs and credentials stay out of this repo.

## Topic map (synthetic examples)

The examples below use invented hostnames and entity names. Replace them with
your private configuration. Never commit production topics or credentials.

### State topics (retained, published by server)

```
nabla/ha/nave/ui/state/light/living_room
nabla/ha/nave/ui/state/light/kitchen
nabla/ha/villa/ui/state/light/pool
nabla/ha/villa/ui/state/light/terrace
```

Pattern: `nabla/ha/<site>/ui/state/<domain>/<entity_name>`

Each site's automation publishes its entities under its own prefix. A panel
subscribing to multiple sites receives state from all of them.

### Command topics (non-retained, consumed by server)

```
nabla/ha/nave/ui/cmd/light/living_room
nabla/ha/nave/ui/cmd/light/kitchen
nabla/ha/villa/ui/cmd/light/pool
nabla/ha/villa/ui/cmd/light/terrace
```

Pattern: `nabla/ha/<site>/ui/cmd/<domain>/<entity_name>`

Commands go to the site that owns the entity. The server validates the command
and updates the entity through its local Home Assistant API.

## State message format

State messages are JSON, retained, and must include all required fields:

```json
{
  "state": "on",
  "available": true,
  "dimmable": true,
  "brightness_pct": 75,
  "ts": 1726627200
}
```

| Field | Type | Values |
|-------|------|--------|
| `state` | string | `"on"`, `"off"`, `"unavailable"`, `"unknown"` |
| `available` | boolean | `true` if the entity can receive commands |
| `dimmable` | boolean | `true` if brightness adjustment is supported |
| `brightness_pct` | integer | 0–100, current brightness percentage |
| `ts` | integer | Unix timestamp (seconds) of this state snapshot |

### Freshness rules

Panels require SNTP synchronization before accepting state. A message is rejected if:

- `ts` is more than 90 seconds in the past (stale), or
- `ts` is more than 5 seconds in the future (clock skew / replay).

Servers must republish retained state at least every 30 seconds to maintain
freshness. Panels disable controls for slots without recent valid state.

### Repeated identical state

Publishing the same state values refreshes freshness without triggering UI
notifications. The panel only notifies views when values actually change.

## Command message format

Commands are JSON, non-retained, and specify the desired state:

```json
{
  "state": "on",
  "brightness_pct": 80
}
```

| Field | Type | Values |
|-------|------|--------|
| `state` | string | `"on"` or `"off"` |
| `brightness_pct` | integer | 0–100, target brightness |

The server must validate payloads before forwarding to Home Assistant. Do not
use legacy receivers that interpret arbitrary payloads as toggles.

### Command confirmation

After sending a command, the panel waits up to 15 seconds for matching
authoritative state. If state arrives confirming the requested on/off and
brightness (within ±1%), the pending indicator clears. Otherwise, a timeout
is logged. Pending state does not block subsequent commands but indicates
unconfirmed delivery.

## Broker configuration

### Allowlist requirement

Configure the broker to forward only declared topic mappings. A panel should
not be able to discover or access topics outside its authorized set.

Example Mosquitto bridge (synthetic):

```
# /etc/mosquitto/mosquitto.conf fragment
connection nave_panel
address mqtt.example.net:8883
bridge_cafile /etc/mosquitto/ca.crt
remote_username panel_user
remote_password !secret from deployment

topic nabla/ha/nave/ui/state/light/+ in 1
topic nabla/ha/nave/ui/cmd/light/+ out 1
topic nabla/ha/villa/ui/state/light/+ in 1
topic nabla/ha/villa/ui/cmd/light/+ out 1
```

### TLS requirement

Production deployments must use TLS with certificate verification. The panel's
`mqtt:` block accepts `broker`, `port`, `username`, `password`, and TLS
options. Credentials belong in the installation's `secrets.yaml`.

### Slot limits

The `mqtt-lights` adapter supports 16 slots (0–15). This matches the model's
fixed array size. Installations with more entities need multiple panels or
a different adapter architecture.

## Panel YAML binding

The device YAML owns topic-to-slot mappings. Import `binding.yaml` once per
light with its slot number and state topic. Supply command topics as a vector.

### Example (synthetic, compile-oriented)

```yaml
substitutions:
  nabla_mqtt_command_topics: >-
    std::vector<std::string>{
      "nabla/ha/nave/ui/cmd/light/living_room",
      "nabla/ha/nave/ui/cmd/light/kitchen",
      "nabla/ha/villa/ui/cmd/light/pool",
      "nabla/ha/villa/ui/cmd/light/terrace",
      "","","","","","","","","","","",""
    }

packages:
  mqtt_transport: !include .nabla-ui/adapters/mqtt-lights/package.yaml
  light_0: !include
    file: .nabla-ui/adapters/mqtt-lights/binding.yaml
    vars:
      slot: 0
      state_topic: nabla/ha/nave/ui/state/light/living_room
  light_1: !include
    file: .nabla-ui/adapters/mqtt-lights/binding.yaml
    vars:
      slot: 1
      state_topic: nabla/ha/nave/ui/state/light/kitchen
  # ... continue for each bound light

mqtt:
  broker: !secret mqtt_broker
  port: 8883
  username: !secret mqtt_user
  password: !secret mqtt_password
  certificate_authority: !secret mqtt_ca
```

Empty strings in the command vector disable sending for that slot. The binding
YAML subscribes to state; the transport YAML handles connection events and
the `control_send` script publishes commands.

## Compact vs regular panels

Both profiles share `transport.yaml` and the same protocol. The regular panel
uses LVGL widgets; the compact panel uses display-API rendering with encoder
navigation. Import `package.yaml` for regular or `compact.yaml` for encoder
panels. The model, freshness rules and confirmation logic are identical.

## Server-side automation (Home Assistant example)

The server automation publishes state and consumes commands. This is a private
installation concern; only the message schema is public.

Conceptual flow:
1. Subscribe to all UI command topics.
2. Validate each command JSON and forward to the target entity.
3. On entity state change, publish retained state JSON with current timestamp.
4. Republish all states on a 30-second interval to maintain freshness.

## What this protocol does NOT cover

- **Entity discovery**: panels receive a fixed slot-to-topic binding at build time.
- **Authentication beyond broker credentials**: the broker enforces access control.
- **Sensor or switch entities**: the current adapter handles lights only.
- **Native API fallback**: MQTT and native API are independent transports.

## Security notes

- Credentials (`mqtt_broker`, `mqtt_user`, `mqtt_password`, `mqtt_ca`) must use
  `!secret` references in private YAML, never hardcoded or committed.
- Topic names can leak installation structure; keep them out of public examples.
- TLS prevents passive interception; broker ACLs prevent unauthorized topics.
- A compromised panel can send arbitrary commands on its authorized topics;
  server-side validation and entity-level rate limits are additional defenses.

## Protocol summary

This protocol connects ESP panels to Home Assistant lights through a shared MQTT
broker without the native API. State topics are retained; commands are not.
The panel checks freshness using SNTP and a maximum age of 90 seconds.
Broker permissions must allowlist topics. Sixteen slots support up to sixteen
lights per panel. Real hosts and credentials stay in private installation YAML.

## Related documentation

- [MQTT lights adapter](../adapters/mqtt-lights/README.md): implementation details
- [Private installations](PRIVATE-INSTALLATIONS.md): ownership boundary
- [GitHub library](GITHUB-LIBRARY.md): consuming the library remotely
- [Connectivity design](platform/CONNECTIVITY.md): Wi-Fi, peers and future work
