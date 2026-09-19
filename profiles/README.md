# Display profiles

Profiles own geometry and presentation defaults; device YAML owns menu content.
Regular profiles use LVGL. Compact profiles use the ESPHome display API without
requiring LVGL or PSRAM. See [UI consistency](../docs/UI-CONSISTENCY.md) for shared
appearance rules and [device adoption](../docs/DEVICE-ADOPTION.md) for validation.

## Current profiles and host fixtures

- **regular:** 480×320 LVGL, tile/list views, 36 px header/footer.
- **portrait fixture:** regular renderer at 320×480 with a two-column grid.
- **tft160:** 160×128 compact color target; four rows, 16 px bars and
  12/16/20 px small/body/large fonts. Optional compact-shell/color.yaml supplies
  color icons, 2×2 tiles and a single-item presentation.
- **tiny:** 128×64 monochrome; three rows, 12 px bars, 10/11/17 px
  small/body/large fonts and a 16 px icon font.
- **readable:** 128×64 monochrome; three rows, 12 px bars, 9/10/17 px
  small/body/large fonts and a 20 px icon font.

These values are build defaults in the corresponding profile YAML. Glyph
geometry also depends on the renderer; font size is not a guarantee of physical
legibility. The mono helper can change presentation state at initialization.

From the repository root:

~~~sh
./simulator/run.sh regular
./simulator/run.sh portrait
./simulator/run.sh tft160
./simulator/run.sh tiny
./simulator/run.sh readable
~~~

Tiny/readable use the simplified tiny example menu; they do not duplicate the
regular example catalog. Applications retain the same navigation contracts,
but fixtures need not expose identical menus.

## Monochrome presentation

The tiny/readable fixtures import compact-shell/mono.yaml. Root presentation
switches between a single large icon with label and a text-only list; there is
no 2×2 monochrome grid. List mode uses three rows and middle-scroll: focus stays
near the center until the beginning or end of the list.

Normal selection uses a play marker beside the focused row. High contrast uses
inversion. In single-item mode, normal presentation has no selection box and
high contrast inverts the panel background. Monochrome output uses black and
white; color TFT profiles are not subject to that palette restriction.

List footer visibility is controlled by compact_list_footer (false for these
profiles); icon presentation retains pagination. Interior footer visibility uses
compact_app_footer. The compact mono triangle has a brief 180-degree spin on
boot/view change; monochrome does not mean animation-free rendering.

## Navigation and detail views

Up/Down selects, Enter activates, Escape returns and Home opens the root in the
host fixture. The physical encoder maps movement/activation/Back through its
hardware package. The current title and status text are not focusable.

Entering a child focuses its first content option. Returning restores content
selection rather than a stale toolbar control. Long selected labels scroll
within their clipped row. Compact detail text supports scrolling; wide-screen
clickable ancestor breadcrumbs remain a regular-renderer feature.

Menu structure is device-owned. The tiny example's Settings/Information routes
are a fixture, not a mandatory hierarchy for every small device.

## Forms and real hardware boundaries

Host Wi-Fi flows are simulated. Compact forms use a character selector;
the optional nabla_wifi_compact component can connect device builds to real
scan/connect operations while preserving declared fallback networks.
See the [adapter](../external_components/nabla_wifi_compact/README.md)
for its configuration and limits.

The compact Wi-Fi/form demo still uses a 128×64 layout. The current ST7735
device menu does not expose that demo; it uses captive-portal provisioning.
Normal compact menus derive their dimensions from the display.

Host viewports validate layout, not OLED contrast, panel refresh timing or
physical input. See the [evidence catalog](../docs/DISPLAY-CATALOG.md#tested-devices)
and [ST7735 notes](../hardware/nodemcu-32s-st7735.md) before treating a profile
as hardware-qualified.

## Implementation ownership

Profile YAML owns dimensions/font substitutions. Compact model.h and render.h
own presentation state and drawing; navigation/focus.h owns shared focus policies.
Extend those shared pieces instead of adding device-specific rendering forks.
