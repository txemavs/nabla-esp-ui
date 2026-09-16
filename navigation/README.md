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

The toolbar remains black with a cyan divider. Its selected text sections
invert; focused interior Home points left. Long paths collapse ancestors into
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
