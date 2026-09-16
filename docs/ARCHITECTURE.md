# Architecture and extension philosophy

The goal is a stable reusable core with an expanding module catalog. A panel
should configure YAML; a new capability should be implemented once as a module.
Home Assistant is optional. Local ESPHome functions, Nabla Edge and peer devices
must fit the same application contracts.

The detailed target design starts at [the platform plan](platform/README.md).
It includes component families, editors, adaptive display/input profiles,
commissioning, peer cooperation, vehicle startup and a gated delivery roadmap.
Proposed YAML is marked as design-only; it is not the current accepted schema.

## Current implementation

examples/hello-world/navigation.yaml is validated by the local nabla_navigation
external component, which emits immutable C++ node descriptors.
navigation/logic.yaml currently owns rendering, navigation and focus.
components/ provides reusable visual includes; simulator/hardware/ supplies host
display, mouse, keyboard and clock adapters. Locale substitutions are build-time.

One tree drives an eight-tile launcher and a scrollable list. Root triangle
toggles the view; the rotation control cycles 90 degrees. Interior triangle
and X return to the parent; root status icons are placeholders.
The header has no divider. A shared bar surface provides header and footer.
The desktop footer displays simulated startup progress then brand and clock.
Application views hide the idle footer. See navigation/README.md.

## Boundary and migration

The current example still uses fixed widget IDs, eight row slots and some
profile-specific metrics. General module manifests, typed bindings, forms,
actual peer services and 128x64 rendering are not implemented.
Do not describe these as completed because an illustrative YAML file exists.

Incrementally extract navigation/focus, operation state and layout policies.
Keep the working example as a regression fixture. Prefer native ESPHome actions;
use a small reusable C++ helper or external component for substantial logic.
Do not copy large lambdas into each device or introduce runtime code loading.

Read [architecture contracts](platform/ARCHITECTURE.md) and
[delivery gates](platform/ROADMAP.md) before expanding the public API.
