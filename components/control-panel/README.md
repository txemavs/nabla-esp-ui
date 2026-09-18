# Control Panel

Dense grid of command buttons and sensor displays, organized into switchable
site sections. Extracted from the Core2 monitor pattern for reusable site control.

## Features

- **Sections/tabs**: Multiple sites switchable with LEFT/RIGHT keys
- **Cell types**: Toggle (on/off), Momentary (press), Sensor (read-only)
- **Dense grid**: 2-4 columns depending on screen width, auto-sized cells
- **Sensor row**: Bottom status bar with live readings
- **Theme support**: Dark/light modes with appropriate color coding

## Usage

Import the package after the shell. Set up sections and cells before navigation.

```yaml
packages:
  shell: !include ../components/shell/package.yaml
  control_panel: !include ../components/control-panel/package.yaml

nabla_navigation:
  tree:
    key: home
    title: Main Menu
    children:
      - key: control
        title: Control Panel
        icon: ${icon_settings}
        action: control_panel
```

Initialize state on boot:

```yaml
script:
  - id: control_panel_init
    then:
      - lambda: |-
          using namespace nabla_control_panel;
          state.words = {"On", "Off", "N/A", "Press"};
          
          Section site;
          site.id = "site_a";
          site.label = "Villa";
          site.cells = {
            {"lamp_1", "Kitchen", CellKind::Toggle, false, 0, "", true},
            {"lamp_2", "Hall", CellKind::Toggle, true, 0, "", true},
            {"temp", "Temp", CellKind::Sensor, false, 22, "°C", true},
          };
          state.sections.push_back(site);
          
          state.on_toggle = [](const char* section, const char* cell, bool v) {
            ESP_LOGI("panel", "Toggle %s/%s -> %s", section, cell, v ? "ON" : "OFF");
          };
```

## Cell Structure

```cpp
struct Cell {
  const char* id;       // Unique identifier for bindings
  const char* label;    // Display text
  CellKind kind;        // Toggle, Momentary, or Sensor
  bool state;           // Current on/off state (Toggle only)
  int value;            // Numeric value (Sensor only)
  const char* unit;     // Unit suffix (Sensor only)
  bool available;       // Grayed out when false
};
```

## Input

| Key       | Action                          |
|-----------|---------------------------------|
| UP/DOWN   | Move between rows               |
| LEFT/RIGHT| Switch sections (with package scripts) |
| ENTER     | Activate focused cell           |
| ESC/Back  | Return to parent menu           |

Touch works for all controls. Section tabs are touch-switchable.

## Callbacks

- `state.on_toggle(section_id, cell_id, new_state)`: Called after toggle
- `state.on_momentary(section_id, cell_id)`: Called on momentary press

Wire these to MQTT commands or ESPHome actions for real device control.

## Substitutions

| Name                        | Default   | Description                          |
|-----------------------------|-----------|--------------------------------------|
| nabla_control_panel_footer  | Control   | Footer label text                    |
| nabla_control_panel_dense   | false     | Enable dense mode for small screens  |

### Dense mode

Set `nabla_control_panel_dense: "true"` for small screens (320×240 or similar).
Dense mode provides a packed Core2-style control grid:
- 5 columns at 320px (vs 4 regular), 4 at 240px
- Smaller tabs (18px height vs 24px)
- Compact sensor row (16px vs 24px)
- Short status text (ON/OFF, `>` for momentary)
- Minimal gaps (2px) and margins
- Cell height ~28px to fit many rows
- Content starts immediately under header bar (no dead space)

## Limits

- Up to 8 visible control cells per section (LVGL row slots)
- Sensor cells render in a separate bottom row
- Grid columns: 4 (>320px), 3 (>200px), or 2 (small screens)
- No MQTT binding included; consumer provides callbacks

## Demo

```sh
./simulator/run.sh control-panel         # 480×320 regular
./simulator/run.sh control-panel-core2   # 320×240 Core2-sized (dense)
```

Opens two synthetic site sections (Villa/Nave) with toggle lights, momentary
alerts, and sensor readings. All data is local mock state with no network.

The Core2 demo uses dense mode for smaller cells and tighter spacing, matching
the M5Stack Core2 320×240 ILI9342 display form factor.

## Files

- `control_panel.h`: Cell/section structures and rendering
- `widget.yaml`: Header export for nabla_runtime
- `package.yaml`: Shell integration and scripts

## Dependencies

- components/shell/package.yaml (regular shell)
- Navigation catalog with action: control_panel support
- Locale strings: tr_cp_* (see locales/es.yaml, locales/en.yaml)

## Status

Host simulator verified. No hardware testing. MQTT/HA bindings are consumer
responsibility. The synthetic demo uses invented site/cell names, not real
production topics.
