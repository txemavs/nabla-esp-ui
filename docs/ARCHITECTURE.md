# Architecture and extension philosophy

The goal is a stable reusable core with an expanding module catalog.
Using the library should mean configuring YAML. Extending it should mean
implementing a module once, documenting it and providing a runnable example.

Panel configuration selects hardware, input capabilities, theme, locale,
applications, navigation and data bindings. Home Assistant is one data source;
local ESPHome entities, MQTT and calculated values must also be possible.

Extension levels:
1. Compose existing packages and widgets with YAML includes.
2. Use native ESPHome actions or a small lambda for local behavior.
3. Add a reusable header or external ESPHome component for substantial logic.
Avoid copying large lambdas between panels.

Modules declare parameters, dependencies, input behavior and limitations.
Examples must distinguish demo data and unimplemented functionality.
A new module should not require changing unrelated modules or the core.

Current milestone:
- Shared application toolbar with Home logo, clickable breadcrumbs and close.
- Sequential focus includes all toolbar actions; touch and Enter share activation.
- Parent navigation restores per-node focus and scroll position.
- English/Spanish build-time locale YAML with matching translation keys.

Remaining:
- Declarative YAML catalog is implemented; expand with typed data bindings.
- Keep traversal algorithms reusable; use an ESPHome external component if
  needed to validate and generate arbitrary node structures during compilation.
- Root composition reads the catalog; lift the fixed eight-row capacity when needed.
- Real data adapters, physical input adapters, editable settings and runtime language switching.
- General window history for cross-links: current close semantics follow the
  parent tree, not an arbitrary application/window stack.

The current toolbar is for the 480 x 320 landscape profile. Compact profiles
will reuse navigation semantics but need their own tested layout.

## Persistent toolbar and appearance
- Toolbar remains black in both light and dark content modes.
- A continuous medium-gray bottom line separates toolbar and content; no boxed borders.
- Focused toolbar controls turn white on the black background.
- Use the shared accent-colored logo component; preserve the original JPG as reference.
- Desktop title is translated Main menu; X is hidden there. Splash remains clean.
- Settings > Appearance switches dark/light at runtime. The preference is stored
  through ESPHome globals; persistence follows the platform save interval.
- Content focus retains the white/gray border convention.

The user tree now lives in examples/hello-world/navigation.yaml. The local
nabla_navigation component validates it at ESPHome compile time and emits data.
No manual generation or numeric node maintenance is required.
