# Appearance controls

The catalog actions toggle_dark and toggle_font are selectable leaf rows.
Touch or ENTER applies the change immediately and retains the row focus.
UP/DOWN selects the row; ESC returns normally. Existing dark/light actions
remain available for compatible compositions.

The dark-mode row shows [x] when enabled and [ ] for light mode.
The font row cycles Ubuntu Mono (default) and DejaVu Sans, showing the current
family. Both text families include Spanish accents and regular/bold variants.
Font Awesome icons retain their dedicated font.

The LVGL renderer stores theme and family in restorable globals. The compact
renderer keeps session state and switches its three native bitmap font sizes.
Persistence on a physical target is not verified by this simulator iteration.

fonts.h maps registered text font families in each UI page without replacing unrelated
icon fonts. Layout measurements use the selected family. The same bounded glyph
sets are compiled for each family; carrying both families increases firmware
font storage. Physical memory impact remains to be measured before deployment.

Example leaf: {key: dark_mode, title: Dark mode, action: toggle_dark}.
Example leaf: {key: font_family, title: Font, action: toggle_font}.
Verified here: host regular, portrait, tiny/readable; no device upload.

Appearance also exposes toggle_borders: enabled by default, with idle borders
#404040 in dark mode and #BFBFBF in light mode. With borders disabled, controls
use inverse foreground/background for focus and retain sequential/touch access.
LVGL menus, toolbar controls, forms, Wi-Fi fields and keyboard use the shared
control_style.h helper. Compact profiles use binary fill inversion.
The setting changes decoration, not the display color capability or font.
Physical e-paper rendering/refresh remains unverified; this is a host UI preview.

Borderless regular lists use 32px rows with 2px gaps, versus 48px/6px
with borders. A 480x320 launcher list with both bars fits seven complete rows
instead of four. Font size and bars are unchanged. Generic form rows compact
to 32px with 2px gaps; keyboard key sizes and native 128x64 readable/tiny
profiles keep their existing legibility limits. Launcher tile gaps become 4px.
