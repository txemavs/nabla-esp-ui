# Library contract v0.1

This document defines what nabla-esp-ui provides as a reusable ESPHome/LVGL
library and what it does not. Pin a tested commit, not a floating branch.

## What the library is

A collection of reusable ESPHome packages, LVGL components, compact display
modules, form primitives, device-info bindings, navigation schemas, fonts, icons
and host fixtures for building panel interfaces. The library works without Home
Assistant; optional adapters integrate MQTT lights, HA cameras and Wi-Fi provisioning.

## What the library is not

- A catalog of real installations, entity bindings or network configurations.
- A source of working secrets, broker addresses or Home Assistant endpoints.
- A plug-and-play device image: consumer YAML owns its menu, hardware and credentials.

## Shared presentation ownership

The device owns menu meaning and actions; Nabla owns the standard rendering and
interaction of its pieces. System applications and embedded Nabla widgets share
global appearance rules. Extend the shared primitive/profile instead of adding
per-screen fonts, borders or input conventions. See [UI consistency](UI-CONSISTENCY.md)
for current behavior, icon/size coverage requirements and the custom-drawing boundary.

## Public entry points

| Entry point | Purpose |
|-------------|---------|
| `packages/regular.yaml` | LVGL-based shell for 320×480+ displays |
| `packages/compact.yaml` | Display-API shell for 128×64–160×128 profiles |

Import one of these via a pinned GitHub package reference. Hardware adapters
(e.g. `hardware/jc3248w535cn.yaml`) are imported separately. Do not import
example menus; the device YAML owns its `nabla_navigation` tree.

## Pinning a revision

Use an immutable full commit SHA:

```yaml
substitutions:
  nabla_ui_ref: <full-40-char-sha>

packages:
  library:
    url: https://github.com/txemavs/nabla-esp-ui
    ref: ${nabla_ui_ref}
    refresh: never
    files:
      - packages/regular.yaml
      - hardware/jc3248w535cn.yaml
```

Do not use `main` or short SHAs for deployed installations. Changing the pin is
a deliberate upgrade; roll back by restoring the previous SHA.

## Consumer responsibilities

The device YAML supplies:

- Device identity (`esphome.name`, `project`)
- Full `nabla_navigation.tree` and `forms`
- `wifi`, `ota`, optional `api` and `mqtt` blocks with `!secret` references
- Hardware adapter import matching the physical board
- Any real entity bindings (MQTT topics, HA cameras, sensors)

The library never reads secrets directly. Examples use placeholder keys like
`!secret wifi_ssid`; create those secrets in your private configuration.

## Examples directory

Files under `examples/` are synthetic fixtures for demonstration and testing.
They use invented networks, brokers and entity names (Site A, Site B, generic
rooms). They do not compile without a `secrets.yaml` stub.

These examples exist to show composition structure, not to serve as deployable
configurations. Copy and customize; do not import them as packages.

## Private device template

[`docs/PRIVATE-DEVICE-TEMPLATE.md`](PRIVATE-DEVICE-TEMPLATE.md) provides a
checklist for creating a real device YAML outside this repository. Keep private
configurations, secrets and firmware binaries in your installation directory.

## Acceptance gate for v0.1.0

Before the owner tags v0.1.0:

- [ ] At least one public example compiles with ESPHome using only a secrets stub.
- [ ] All example READMEs state they are synthetic fixtures.
- [ ] No real installation names, entities or credentials in public files.
- [ ] `packages/regular.yaml` and `packages/compact.yaml` compile independently.

## Related documentation

- [GitHub library setup](GITHUB-LIBRARY.md): remote package consumption details
- [Private installations](PRIVATE-INSTALLATIONS.md): ownership boundary
- [Platform roadmap](platform/ROADMAP.md): milestone status and exit gates
