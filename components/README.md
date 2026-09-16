# UI component catalog

Reusable UI building blocks live here, grouped by component. New components
should have a widget.yaml entry point and a README describing parameters,
dependencies, focus/pressed behavior, examples and limitations.

- logo/widget.yaml: accent-colored equilateral triangle with down/left motion and white focus.
- toolbar/widget.yaml: persistent application shell, breadcrumbs and close.
- menu-tile.yaml: parameterized application tile.

Keep appearance in components and theme, input bindings in hardware profiles,
and navigation actions in navigation/. Compose applications from these pieces.
Components may be composed into larger patterns without duplicating their logic.
The toolbar.yaml compatibility entry point remains supported.

The tile slots are populated from the declarative navigation catalog; do not
duplicate application titles, icons or numeric targets in component includes.

- bar/surface.yaml: common header/footer surface and height.
- progress/widget.yaml: shared desktop footer with gray/cyan operation progress.

The [planned component families and extension recipe](../docs/platform/COMPONENTS-AND-INPUT.md)
describe future controls. Inclusion there is not a claim of implementation.

## Native password keyboard preview

The experimental composition in examples/password/ui.yaml uses ESPHome's
native LVGL textarea and keyboard. See simulator/password.yaml and
simulator/README.md to run it. It is a fixed 480x320 fixture, not a registered
production editor or a compact OLED renderer.
Input: touch or sequential Up/Down/Enter; Escape clears/cancels.
States: editing, demo accepted, cancelled. Input is masked and limited to 63
characters; accepting/cancelling clears it. No persistence, network or logging
of entered text. Uses the shared fonts/logo and native Montserrat keyboard
symbols. Spanish/English labels compile; native keys keep the stock repertoire.
