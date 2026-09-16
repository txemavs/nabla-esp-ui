# Declarative navigation

Edit examples/hello-world/navigation.yaml to define the menu. It uses the
local nabla_navigation ESPHome external component; no standalone generator
command is required.

Each node has:
- key: unique stable lowercase identifier (letters, digits, underscores).
- title: text or a locale substitution.
- icon: Font Awesome glyph substitution (required for desktop entries).
- children: optional nested nodes.
- detail: optional informational leaf text.
- action: open (default), dark or light. Theme actions are leaves.

Example inside a node's children:

```yaml
- key: workshop
  title: Workshop
  children:
    - key: workshop_temperature
      title: Temperature
      detail: No sensor connected
```

Use a locale key instead of literal text for bilingual panels. Adding this
folder needs no C++ changes. Actual sensor bindings are not implemented yet;
detail is informational text, not a sensor configuration.

The compiler checks unique keys, required titles, known fields/actions, maximum
eight children per node, sixteen levels and 128 total nodes. Nesting defines
parents; numeric indices are internal and may change freely when nodes move.
The launcher reads the root children, titles and icons from the same catalog.
It renders up to eight tiles and hides unused slots.

C++ in external_components/nabla_navigation/navigation.h contains reusable
traversal. catalog.py validates YAML and emits escaped, immutable data.
The appearance action uses semantic action codes, not hardcoded node indices.
navigation/tree.h is a compatibility include; translations.yaml is retired.

The toolbar uses the tree to calculate its route. Home opens the desktop,
breadcrumbs jump to ancestors, X/ESC returns to the parent. It restores focus
and scroll per node. This is parent navigation, not arbitrary window history.
UP/DOWN focus order is content, Home, visible breadcrumbs, X. On the desktop,
only content and Home participate; X is hidden. ENTER and touch share activation.

The toolbar follows the active theme with a medium-gray divider. Its selected controls
turn white; focused Home gains a white border. Long paths collapse ancestors into
a clickable ... parent link. Layout is currently tested at 480 x 320.

## Validation

```sh
python3 -m unittest discover -s tests -p 'test_*.py'
source .venv/bin/activate
esphome compile simulator/hello-world.yaml
esphome -s ui_language en compile simulator/hello-world.yaml
```

The tests compile generated C++ and exercise traversal and validation failures.
See locales/README.md and navigation/INPUT.md for shared contracts.

Current presentation contract: ordinary control focus changes only borders, never icon, text or
background colors. Icons and names default to white. The current toolbar title
is bold in the theme foreground color and excluded from focus; ancestors remain regular clickable text.
The single underline starts just after the logo's lower tip. Startup spins the
same toolbar logo in place for one second; there is no separate splash page.
Set nabla_monochrome: "true" to hide unselected control borders. This is a
presentation option, not validation of physical OLED hardware.

## Per-card colors

Desktop entries accept optional 24-bit integer bg_dark and bg_light fields.
Settings demonstrates bg_dark: 0x12345A and bg_light: 0x9ABCE3.
These are presentation values in the navigation YAML, not hardcoded node indices.
Icons currently use the registered Font Awesome glyph. Arbitrary images and
animated artwork are planned tile content variants; they are not implemented
by the catalog yet.

The brand triangle is the focus exception: inside a view it rotates 90 degrees
left over 180 ms, without a focus box. Activating it returns to the parent,
just like X/ESC. Removing focus restores its downward orientation. At the root
it remains the Home mark. The underline meets its resting lower tip.

## Launcher views and orientation

At the root, the triangle toggles tiles/list. Both use the same YAML tree.
Up/Down traverses items, then the triangle and rotation button; Enter activates.
List focus scrolls into view and the root scroll position survives application
entry and Back. Inside applications the triangle retains its Back behavior.
The circular-arrow button at the root rotates by 90 degrees, cycling all four
orientations. LVGL software rotation changes the logical viewport; SDL retains
its physical 480x320 window. Tiles reflow 4x2 in landscape and 2x4 in portrait.
Input mapping is handled by ESPHome; orientation.h bounds stale released
pointer coordinates after rotation without altering pressed touch coordinates.

This is not yet a validated 128x64 profile: that needs compact bars/fonts,
reduced status content and a physical display/input adapter. The shared list
and navigation are the reusable basis, not evidence of hardware support.
