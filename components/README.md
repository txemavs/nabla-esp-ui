# UI component catalog

Reusable UI building blocks live here, grouped by component. New components
should have a widget.yaml entry point and a README describing parameters,
dependencies, focus/pressed behavior, examples and limitations.

- Optional external_components/nabla_wifi: persistent primary Wi-Fi adapter for
  the LVGL form; see its README for limits and fallback guarantees.
- device-info/package.yaml: read-only model, network, version and uptime snapshot;
  see device-info/README.md for field bindings and limits.
- logo/widget.yaml: theme-foreground equilateral triangle with upward focus motion.
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

## Manual Wi-Fi form

[Wi-Fi form](wifi-form/README.md): packaged two-field SSID/password editor,
native keyboard, validation, sequential focus and cancel/return lifecycle.
Selected by action: wifi_demo in the demo tree. Current support is host-only
480x320/320x480, with simulated connection states and no radio or persistence; see its contract for details.

Launcher entries accept icon_dark/icon_light independently of optional bg_dark/bg_light.
Default surfaces are black/white; focus and text invert with the theme.

## Compact shell

[Compact shell](compact-shell/README.md) renders the shared catalog at 128x64
with three-row and readable single-row profiles, one-bit fonts and shared focus
policies. Host-verified; physical OLED and GPIO adapters remain pending.

## M2 Wi-Fi workflow and compact editor

[Forms primitives](forms/README.md): validated YAML fields, transactional numeric,
choice/toggle editors, confirmation/error states, UTF-8 draft and optional Latin
keyboard. Regular/compact renderers share the controller and simulated Wi-Fi
state machine. The contract documents dependencies, states, focus, bounds and examples.

- appearance/fonts.h: paired proportional/monospace text fonts; Appearance uses
  toggle_dark and toggle_font rows. See appearance/README.md for state, input,
  dependencies and host verification limits.

- nabla_navigation/control_style.h: shared LVGL theme-relative borders and inverse
  focus, selected with Appearance > Borders; see appearance/README.md.

- shell/package.yaml: reusable regular LVGL shell without an imported menu.
  The consumer owns nabla_navigation; see shell/README.md and simulator/composition.yaml.

## Build-time packaging

[Runtime helper exporter](../external_components/nabla_runtime/README.md) resolves
shared C++ headers from local or pinned GitHub checkouts. It adds no UI controls;
[the GitHub entry point](../docs/GITHUB-LIBRARY.md) assembles the public LVGL shell.

## Optional Home Assistant adapter

[Clock and Wi-Fi status](../adapters/homeassistant/README.md): native time sync,
one-second gray/green/blue Wi-Fi indicator, no input controls. Requires regular
shell, Wi-Fi and native API; credentials stay in the consumer configuration.

## Command rows

[Device-owned commands](../navigation/COMMANDS.md) bind a menu leaf to native
ESPHome actions with live state and availability. No installation entities are
part of the library. Supports shared touch/keyboard/compact activation.
