# Direct Home Assistant light panels

Experimental ESP32 / ESP-IDF HTTPS adapter, independent of MQTT and the inbound
ESPHome native API. Each nabla_ha instance has its own HTTPS origin, private token
and an explicit list of up to eight light/switch entity IDs.

Import package.yaml alongside the regular shell and light-card/panel.yaml
(include the light panel once; mqtt-lights already includes it). Supply
nabla_ha_clients, nabla_ha_root_nodes and nabla_ha_view_index in the device YAML.
Use ha_light_open(site, slot, title) for activation and call ha_panel_status
after the shared light panel rendering. Device-owned light-state expressions
must select the appropriate client; MQTT state must never feed these routes.

The client reads /api/states/<entity> and posts explicit turn_on/turn_off service
calls. It does not optimistically alter state. A worker performs network I/O;
only the ESPHome main loop updates UI-visible state through bounded queues.
TLS certificates and hostnames are verified; redirects are disabled so bearer
credentials cannot be forwarded to another origin. Bodies are capped at 8 KiB.

Connection states distinguish connecting, ready, unreachable, unauthorized and
invalid response. Readiness requires an authenticated API response. Unknown or
stale entity states cannot execute actions. Offline site icons are gray, and
their detail view explains the reason and origin without showing credentials.

One worker per instance (8 KiB stack), one pending request/result per worker,
and a shared TLS mutex to limit peak memory. Healthy polling advances one entity
every 600 ms after each response; failures retry after 8 seconds. Entity state
expires after 30 seconds. This is polling, not a live WebSocket subscription.
Do not configure large numbers of instances on memory-constrained devices.

The credential must be private and have access to the configured entities.
Native ESPHome integration is still independently usable for cameras and time.
The compact renderer and host simulator do not implement this transport.
