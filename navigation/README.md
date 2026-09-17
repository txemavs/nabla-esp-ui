# Declarative navigation

The accepted configuration is examples/hello-world/navigation.yaml, validated by
the local nabla_navigation ESPHome external component during compilation.
The future platform schema in docs/platform/ is not accepted by this component.

## Current node fields

- key: unique lowercase identifier using letters, digits and underscores.
- title: text or a locale substitution; must not be empty.
- icon: Font Awesome glyph substitution, required for root children.
- children: nested entries, at most eight per node.
- detail: informational text, not an entity binding.
- action: open (default), dark or light; theme actions are non-root leaves.
- bg_dark / bg_light: optional 24-bit RGB integers for desktop tile backgrounds.

The tree allows sixteen levels and 128 nodes. Unknown fields, duplicate keys
and invalid references/types fail validation. Numeric indices are generated
implementation details; applications must not hardcode them.

Example child entry using currently accepted fields:

```yaml
- key: workshop
  title: Workshop
  children:
    - key: workshop_temperature
      title: Temperature
      detail: No sensor connected
```

Use registered tr_* locale keys for bilingual panels. Register required icons
and glyphs before referencing them. Adding nodes does not implement sensor data;
typed bindings remain planned.

## Views, focus and actions

The same root children populate up to eight tiles or eight reusable list rows.
The root triangle toggles tiles/list. Inside a view it returns to the parent,
as does X/ESC. Keyboard Home opens the root without changing the chosen view.

UP/DOWN traverse content, triangle, then root rotation or interior ancestor
breadcrumbs and X. The current title and Wi-Fi/Bluetooth placeholders are not
focusable. ENTER and touch share a 120 ms activation-feedback path.
See [input contract](INPUT.md).

List focus scrolls into view; per-node focus and scroll survive nested entry.
This is a parent-tree model, not general application-window history.
Focused triangle turns white without a box and rotates left inside views.
Other controls change their border. Current title is bold in the theme foreground.

The root circular-arrow button cycles all four orientations. LVGL software
rotation changes logical dimensions, not the physical SDL window. Tiles reflow
4x2 or 2x4. orientation.h bounds stale released-pointer coordinates after rotation.

## Shell

Header/footer share a 36 px bar surface. Header has no line or progress.
The desktop footer is permanent: simulated startup progress, then gray
Soluciones lógicas NABLA and HH:MM. Interior views hide the idle desktop footer.
Progress may temporarily show it. Gray/cyan progress is not a real boot measure.

The current layout is validated in the 480x320 host window with software
rotation. A tiny 128x64 layout is pending; see [profile plan](../docs/platform/COMPONENTS-AND-INPUT.md).

## Implementation and checks

catalog.py emits immutable node data; navigation.h supplies traversal.
logic.yaml still contains example-specific rendering and fixed row IDs.
navigation/tree.h is a compatibility include.

```sh
python3 -m unittest discover -s tests -p 'test_*.py'
source .venv/bin/activate
esphome compile simulator/hello-world.yaml
esphome -s ui_language en compile simulator/hello-world.yaml
```

For input/layout changes also test root toggle, last-row scroll, nested return,
rotation and rotated touch in the actual simulator. Long-title fitting is a
prototype behavior; readable minimum font policies belong to the future profiles.

## Manual Wi-Fi editor route

The demo tree contains Settings > Connections > Wi-Fi. A leaf with
action: wifi_demo opens the packaged form instead of the generic leaf page.
Theme actions remain dark/light and still return to their parent.
While the form is open, sequential input is routed to its editor; toolbar
navigation remains available by touch, with Cancel reachable sequentially.
See ../components/wifi-form/README.md. This is simulated local validation,
not credential persistence or live network configuration.

Toolbar breadcrumbs use compact measured labels separated by non-focusable gray
chevrons. The current title stays bold; collapsed ancestors retain a parent link.
The focused parent triangle rotates 60 degrees so the next vertex points up.

- Entering a child menu focuses its first content option and starts at the top.
  Returning may restore a content selection, but never a saved toolbar control
  when the destination has options. Empty leaves can focus Back; the Wi-Fi
  editor starts at SSID. Same-page view/rotation updates preserve current focus.

## Neutral surfaces and colored icons

Default bg_dark/bg_light are black/white. Explicit background overrides remain
supported, but the example uses none. icon_dark/icon_light are optional 24-bit
RGB values: they default to white/black. The demo gives each app an icon palette,
with Settings blue. Labels, focus borders and the navigation logo follow the
theme foreground. Wi-Fi fields, buttons and keyboard also invert.
nabla_monochrome ignores tile color overrides and renders icons in foreground;
this is a presentation mode, not validated e-paper hardware support.

## Optional real Wi-Fi and information

The wifi action opens the Wi-Fi form; wifi_demo remains an alias for compatibility.
The transport is mock unless the private ESP32/LVGL composition enables nabla_wifi.
See external_components/nabla_wifi/README.md for save, cancellation and fallback
behavior. info fields bind read-only information leaves; see components/device-info.

Appearance supports toggle_dark and toggle_font leaf actions. They apply in
place and retain focus; the row shows the current checkbox/family state.

Appearance also exposes toggle_borders: enabled by default, with idle borders
#404040 in dark mode and #BFBFBF in light mode. With borders disabled, controls
use inverse foreground/background for focus and retain sequential/touch access.
LVGL menus, toolbar controls, forms, Wi-Fi fields and keyboard use the shared
control_style.h helper. Compact profiles use binary fill inversion.
The setting changes decoration, not the display color capability or font.
Physical e-paper rendering/refresh remains unverified; this is a host UI preview.

## Optional view integration

Generated nabla::routes::node_KEY constants refer to declared routes without numeric indices or translated titles. Regular shell hooks nabla_view_refresh (statement) and nabla_back_intercept (optional early-return statement) default to no-ops. Import optional views after the shell; combine hooks explicitly when composing multiple views.

Optional editors can capture U/D/ENTER through nabla_input_captured,
nabla_move_handler and nabla_activate_handler; defaults preserve shell behavior.
When multiple views are imported, the root composition must explicitly chain
their refresh/Back hooks. A package import alone must not silently remove a view.

## Interaction performance

Regular controls select on press and activate on click after a 40 ms feedback
interval. With borders enabled, content-to-content focus changes update only the
previous and next border. Toolbar transitions and borderless inversion retain
the full renderer for correctness. Font-family traversal runs on page/family
changes, and already matching fonts are not reassigned.
The ui_perf log reports CPU time spent in focus/render callbacks, not display
latency or touch-to-photon time. Network workers never modify LVGL objects.
