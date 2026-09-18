# nabla_navigation

Declarative navigation tree and form metadata code generator for ESPHome panels.
Validates YAML structure at compile time and emits immutable C++ data structures
consumed by shell renderers.

[Back to external_components](../README.md)

## What it does

- Validates navigation tree structure (keys, titles, actions, nesting depth).
- Validates optional form field definitions (number, choice, toggle).
- Emits `nabla::nodes[]` array with parent links and action codes.
- Emits `nabla::routes::node_*` constants for compile-time route references.
- Emits `nabla_forms::fields[]` array for generic settings forms.
- Registers command triggers for interactive leaves.

This is a build-time component with no runtime code. Shells import the generated
headers and implement traversal, focus and rendering.

## Configuration

```yaml
nabla_navigation:
  tree:
    key: root
    title: ${tr_home}
    children:
      - key: settings
        title: ${tr_settings}
        icon: ${fa_cog}
        children:
          - key: info
            title: ${tr_information}
            info: model
          - key: appearance
            title: ${tr_appearance}
            children:
              - key: dark_mode
                title: ${tr_dark_mode}
                action: toggle_dark
  forms:
    - key: brightness
      label: ${tr_brightness}
      type: number
      min: 0
      max: 100
      step: 5
      initial: 50
    - key: theme
      label: ${tr_theme}
      type: choice
      options: ["Dark", "Light", "Auto"]
      initial: 0
  commands:
    - node: some_command_leaf
      available: |-
        return true;
      state: |-
        return "Ready";
      on_press:
        - logger.log: "Command executed"
```

### Tree schema

| Field | Required | Description |
|-------|----------|-------------|
| `key` | yes | Unique lowercase identifier (`[a-z][a-z0-9_]*`) |
| `title` | yes | Display text (use locale substitutions) |
| `icon` | root children | Font Awesome glyph (required for desktop tiles) |
| `detail` | no | Static informational text |
| `action` | no | Leaf action (default: `open`) |
| `children` | no | Nested entries (max 8 per node) |
| `info` | no | Live info field: `model`, `wifi`, `ip`, `version`, `uptime` |
| `bg_dark` | no | 24-bit RGB tile background (dark theme) |
| `bg_light` | no | 24-bit RGB tile background (light theme) |
| `icon_dark` | no | 24-bit RGB icon color (dark theme) |
| `icon_light` | no | 24-bit RGB icon color (light theme) |

### Supported actions

| Action | Description |
|--------|-------------|
| `open` | Navigate into children (default) |
| `dark` | Switch to dark theme |
| `light` | Switch to light theme |
| `toggle_dark` | Toggle dark/light theme in place |
| `toggle_font` | Cycle font family in place |
| `toggle_borders` | Toggle focus border style |
| `wifi_demo` / `wifi` | Open Wi-Fi editor |
| `forms_demo` | Open generic forms editor |
| `command` | Execute registered command trigger |
| `control_panel` | Open control panel view |

### Form field schema

| Field | Required | Description |
|-------|----------|-------------|
| `key` | yes | Unique lowercase identifier |
| `label` | yes | Display label (1–96 UTF-8 bytes) |
| `type` | yes | `number`, `choice`, or `toggle` |
| `initial` | yes | Default value |
| `min` | number | Minimum value (int32) |
| `max` | number | Maximum value (int32) |
| `step` | number | Increment step (default: 1) |
| `options` | choice | List of 1–8 option labels |

### Command schema

| Field | Required | Description |
|-------|----------|-------------|
| `node` | yes | Key of a `command` action leaf |
| `available` | yes | Lambda returning `bool` |
| `state` | yes | Lambda returning `std::string` |
| `on_press` | yes | ESPHome automation |

## Limits

- Maximum 128 nodes total.
- Maximum 16 nesting levels.
- Maximum 8 children per node.
- Maximum 8 form fields.
- Maximum 8 choice options per field.

## Generated output

The component emits:

```cpp
namespace nabla {
  const Node nodes[] = { ... };
  const int count = N;
  namespace routes {
    constexpr int node_root = 0;
    constexpr int node_settings = 1;
    // ...
  }
}

namespace nabla_forms {
  const Field fields[] = { ... };
  const int field_count = M;
}
```

Use `nabla::routes::node_*` constants instead of hardcoded indices.

## Related documentation

- [navigation/README.md](../../navigation/README.md) — navigation contracts and focus model
- [docs/ARCHITECTURE.md](../../docs/ARCHITECTURE.md) — implementation boundary
- [docs/LIBRARY-CONTRACT-v0.1.md](../../docs/LIBRARY-CONTRACT-v0.1.md) — library versioning
