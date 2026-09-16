# Display profiles

M1 provides three named profiles and a regular portrait fixture. All consume
examples/hello-world/navigation.yaml; none duplicates the menu or entity mapping.

- regular: LVGL shell at 480x320, tile/list switch, 36 px header and footer.
- tiny: actual 128x64 monochrome display, three list rows, 10 px body font.
- readable: actual 128x64 monochrome display, one row, 16 px body font.
- portrait fixture: regular shell at an actual 320x480 viewport, two-column grid.

Start from the repository root:
```sh
./simulator/run.sh regular
./simulator/run.sh portrait
./simulator/run.sh tiny
./simulator/run.sh readable
```

The compact renderer uses ESPHome's display API and 1-bit fonts, avoiding an
LVGL dependency for the small-display view. Its SDL viewport remains 128x64.
It does not emulate a physical OLED driver, panel contrast or refresh timing.
The regular renderer remains LVGL and retains its integrated Wi-Fi form.

## Navigation and presentation

Up/Down selects, Enter activates, Escape returns, Home opens the root.
Touch selects the corresponding visible row. The current title is not clickable.
The compact header triangle is reachable after the content entries:
at root it switches tiny/readable, inside it returns to the parent.
A leaf also shows an explicit Back row; its action is the same header target.
The footer and status text do not participate in focus.

Entering a child starts at the first option. Returning restores a content
selection and scroll anchor, never a stale toolbar selection. Changing density
keeps the current route and ensures the selected content fits in the viewport.
Long selected labels scroll within their clipped row; they do not move focus.

Both compact bars are 12 px high. Root footer is persistent; application footer
defaults off and can be enabled with compact_app_footer: "true".
Root content has 40 px: three 13 px rows (one spare pixel) or one 40 px row.
Without footer, application content has 52 px: three 17 px rows or one 52 px row.
Fonts are 8 px bold for title/footer, 10 px for tiny rows, 16 px for readable rows.
Their one-bit raster output has been checked at native resolution. Physical
legibility still depends on the OLED size and viewing distance.

Settings > Appearance changes black/white polarity locally on either renderer.
Compact output ignores RGB palettes: icons/selection/text use foreground,
background uses its inverse. Captured output contains exactly two RGB values,
black and white. OLED marks are pixel-aligned outlines; no smoothing or animation
is used in the compact renderer.

## Boundaries

Wi-Fi is the same route in all profiles. Regular uses the native keyboard;
compact now uses the M2 character selector and shared simulated Wi-Fi workflow.
No radio, credentials or backend is needed for M1. The compact header presents
the current location; wide-screen clickable ancestor breadcrumbs remain a
regular-profile feature. Leaf descriptions are clipped to the compact detail
region; a paged detail reader is future component work.

Runtime metrics live in geometry.h. Input wrapping/content restoration/scroll
policies live in navigation/focus.h. The compact state machine is in
components/compact-shell/model.h and the renderer in render.h. These are
bounded compile-time menu views, not a dynamic menu download implementation.
