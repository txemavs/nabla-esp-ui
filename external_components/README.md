# ESPHome external components

This folder contains ESPHome `external_components` sources published with the
nabla-esp-ui library. Consumer YAML pulls them via a pinned package reference;
see [docs/LIBRARY-CONTRACT-v0.1.md](../docs/LIBRARY-CONTRACT-v0.1.md) for the
full import pattern.

## Pinning external components

Import components using the same SHA as your package reference:

```yaml
substitutions:
  nabla_ui_ref: <full-40-char-sha>

external_components:
  - source:
      type: git
      url: https://github.com/txemavs/nabla-esp-ui.git
      ref: ${nabla_ui_ref}
      path: external_components
    components: [nabla_navigation, nabla_runtime]
    refresh: never
```

Do not use `main` or short SHAs for deployed installations.

## Component summary

| Component | Role | README |
|-----------|------|--------|
| `captive_portal` | Branded/forked ESPHome captive portal with network list and OTA | [README](captive_portal/README.md) |
| `nabla_camera_fetch` | Background HTTPS JPEG fetcher for HA camera snapshots | [README](nabla_camera_fetch/README.md) |
| `nabla_ha` | Optional Home Assistant REST API client for light/switch control | [README](nabla_ha/README.md) |
| `nabla_navigation` | Declarative navigation tree and form metadata codegen | [README](nabla_navigation/README.md) |
| `nabla_runtime` | Build-time C++ header exporter for reusable modules | [README](nabla_runtime/README.md) |
| `nabla_wifi` | Optional primary Wi-Fi provisioning adapter | [README](nabla_wifi/README.md) |

Note: `captive_portal` is a forked/branded ESPHome component that replaces the
built-in version; the others are Nabla-specific implementations.
