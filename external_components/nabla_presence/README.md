# MQTT presence and address discovery

Optional `nabla_presence` announces a stable ESP32 identity and its current Wi-Fi
IPv4 address to Nabla Control. It works with either compact, LVGL mirror or
responsive web renderers. It does not carry frames, audio or remote input.

```yaml
# Keep the existing Wi-Fi, MQTT credentials and renderer configuration.
external_components:
  - source: github://txemavs/nabla-esp-ui@main
    components: [nabla_presence]
nabla_presence:
  topic_prefix: nabla/discovery
```

Requires `esp32`, `wifi`, an existing `mqtt` configuration, and a Nabla renderer
for Control adoption. Adds `web_server_base` and `/nabla/identity`; it cannot be
combined with ESPHome's separate `web_server`. The HTTP service uses port 80.
Omit this component to disable announcements. The default topic prefix is
`nabla/discovery`; no wildcards, credentials or per-installation hosts belong here.
Pin the Git revision in reproducible deployments.

## Wire contract v1

Topic: `<prefix>/esp32-<12 lowercase MAC hex digits>/announce`, QoS 1, retained.

```json
{"v":1,"device_id":"esp32-001122aabbcc","boot_id":"0123456789abcdef","name":"example-panel","host":"192.0.2.20"}
```

A 5-second poll publishes after Wi-Fi/MQTT connection, reconnection or IP change,
and at least once each minute while connected. Failed publishes retry on the
next poll. A random 64-bit `boot_id` changes on every boot; the MAC-derived device
identity survives Wi-Fi changes and normal firmware updates. Hardware replacement
or a configured MAC change is a new identity. MQTT credentials, birth/will topics,
and existing application topics are preserved.

`GET /nabla/identity` returns `v`, `device_id`, `boot_id` with `Cache-Control:
no-store`. It is read-only, with no tokens or credentials. Identity fields are
immutable after setup; MQTT callbacks only mark reconnect intent, and publishing
runs on the component loop. There are no input gestures or UI states to render.

Nabla Control 0.9+ enables this independently in its MQTT tab. An administrator
explicitly links a discovered identity to an existing device (preserving its
entry, dashboard and button IDs), or adds a new one. Subsequent address changes
are accepted only after the HTTP endpoint confirms the same identity and boot,
and the renderer responds. Old retained announcements are hints, never proof of
availability. No HTTP route means no automatic update: MQTT does not create
Tailscale routing, DNS or a tunnel.

## Bounds and trust

Control accepts up to 64 announcements of at most 1 KiB, expiring after 180 seconds
without a new announcement. It checks at most four changed devices per 15-second
pass, with bounded HTTP reads/timeouts and no redirects. IPv4 only in v1.
The MQTT broker and LAN are trusted: IDs and boot values detect stale/wrong
endpoints, not malicious impersonation. Use broker ACLs so each device can publish
only its own announcement topic. Do not expose the HTTP service to the internet.
Omitting the component does not delete its retained broker message; an old hint
expires in Control and fails the HTTP boot/identity check after replacement.

## Validation

ESPHome 2026.8.2 / ESP-IDF ESP32-S3 compile passed. HA tests exercise stale hints,
identity mismatch, opt-out, collisions, bounded inventory and preserved IDs; the
browser fixture covers explicit linking and independent settings. Physical Wi-Fi
roaming is still to be qualified; compile results are not a hardware claim.
