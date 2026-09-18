# Display profiles

M1 provides three named profiles and a regular portrait fixture. All consume
examples/hello-world/navigation.yaml; none duplicates the menu or entity mapping.
The tiny profile uses examples/tiny/navigation.yaml for a simplified menu.

- regular: LVGL shell at 480x320, tile/list switch, 36 px header and footer.
- tiny: actual 128x64 monochrome, two root views only: fullscreen icon or text list.
- readable: actual 128x64 monochrome, single-row readable starting in icon view.
- portrait fixture: regular shell at an actual 320x480 viewport, two-column grid.

Start from the repository root:
```sh
./simulator/run.sh regular
./simulator/run.sh portrait
./simulator/run.sh tiny
./simulator/run.sh readable
```

The compact renderer uses ESPHome's display API and 1-bit fonts, avoiding an
LVGL dependency for the small-display view. The tiny/readable SDL viewports remain 128x64.
It does not emulate a physical OLED driver, panel contrast or refresh timing.
The regular renderer remains LVGL and retains its integrated Wi-Fi form.

Tiny and readable use `single_icon_mode` from `components/compact-shell/mono.yaml`:
the root menu toggles between fullscreen icon (readable=true) and text-only list
(readable=false). List mode shows plain text rows without icon glyphs.
There is no intermediate 2x2 tile grid on 128x64.
The tft160 profile retains the 2x2 color tiles and list icons via `color.yaml`.

## Tiny (128×64 mono OLED) UX Philosophy

The tiny profile encodes a distinct philosophy for 128×64 mono OLED displays,
prioritizing readability on severely constrained screens:

### Two root views only
Fullscreen icon OR text list. No 2×2 grid—space is too constrained.

### Appearance modes
Renamed from "Borders" to **Normal / Alto contraste** in the tiny context:
- **Normal** (borders=true): ▶ play marker on the LEFT of focused row, no box
  borders. Clean appearance with minimal visual chrome.
- **Alto contraste** (borders=false): current inverted/bar selection. Filled
  background for selected items maximizes visibility.
- **Single-item (icon) view**: Alto contraste inverts the panel background;
  Normal shows no selection chrome (only one item visible anyway).

### Typography floor
Never use undersized fonts on tiny displays. Minimum font size = form body size
(SSID/password row). In icon view, the single-item label uses the large font
(«Conexiones» size), centered at the bottom. Icon is above, well-packed with
less empty top space.

### List mode layout
- No footer—all vertical space for content.
- 3 rows visible, text-only (no icon glyphs in list).
- Middle-scroll: focus stays center row until reaching list boundaries.

### Icon/single mode layout
- HAS toolbar+footer (shows n/m pagination index).
- Smaller centered icon (16px vs old 20px) for better label space.
- Large font for label, centered below the icon.

### Nabla triangle animation
On boot and when toggling view from main menu, the header triangle spins
~180° as a brief visual acknowledgment of the mode change.

### Settings structure (Kit1 ESPUI shape)
```
Ajustes (Settings)
├── Conexiones
│   └── Wi-Fi (captive portal hint)
├── Apariencia
│   ├── Oscuro (toggle dark/light)
│   ├── Fuente (toggle font family)
│   └── Alto contraste (toggle play marker / inverted bar)
└── Información
    ├── Modelo
    ├── Wi-Fi
    ├── IP
    ├── Encendido (Uptime)
    └── Idioma (last—moved from outer menu)
```

### Menu content (public example)
Removed placeholders not useful on tiny (Photos, Music, Cameras, Weather,
Lights, Sensors). Added Control > Site A with four generic lights (Storage,
Office, Bench, Spotlight) mirroring the Kit1 pattern with Site A names.

### Default behavior
Settings and other menus default to list view; can toggle to centered large
single-item view anywhere. Same two-view rule applies throughout on tiny.

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

Both compact bars are 12 px high. The tiny profile controls footer visibility
with `compact_list_footer`: list mode at root hides the footer (3 rows × 17 px
= 51 px content), while fullscreen icon mode keeps footer + pagination.
Interior/app footer defaults off and can be enabled with `compact_app_footer`.

Fonts are 9 px for header/footer/label, 10 px for list body, 17 px for large.
Icon size is 20 px (`compact_icon_size`), vertically centered with its label.
Their one-bit raster output has been checked at native resolution. Physical
legibility still depends on the OLED size and viewing distance.

## Middle-scroll behavior (tiny)

List mode uses middle-scroll: the selected item stays in the middle row while
navigating through the list. At boundaries the selection moves to edge rows,
providing a visual cue when reaching the start or end. The `scroll_anchor_middle`
function in `navigation/focus.h` implements this logic.

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


## Landscape TFT / encoder

The `tft160` profile uses 160x128 logical pixels, four rows, 16 px bars and
12/16/20 px small/body/readable fonts. Build `simulator/tft160.yaml` to inspect
the shared display renderer at native size. Tiny/readable defaults are unchanged.
The compact shell accepts `compact_rows`, `compact_bar_height` and the three
`compact_*_size` substitutions. Normal menus derive width/height from the display.

See [NodeMCU ST7735](../hardware/nodemcu-32s-st7735.md). The current Wi-Fi/form
demo renderer retains its 128x64 layout and is not exposed by this device menu.
