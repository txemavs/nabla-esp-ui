# Toolbar

Shared theme-aware shell with medium-gray bottom divider, drawn Home logo, breadcrumb
segments and X. The desktop has a title and no X. Splash hides the toolbar.

The Home logo points left when focused in an application, without inverting
its background. Home still opens the desktop; X returns to the parent.
Other focused controls change only their border. Touch and sequential input share activation.

Depends on navigation actions/state and theme fonts/colors.
Current widget IDs are shared singleton IDs; include this toolbar once in
LVGL top_layer. It is not yet a multi-instance toolbar.
See navigation/README.md for focus order and breadcrumb truncation.

Current presentation contract: ordinary control focus changes only borders, never icon, text or
background colors. Icons and names default to white. The current toolbar title
is bold in the theme foreground color and excluded from focus; ancestors remain regular clickable text.
The single underline starts just after the logo's lower tip. Startup spins the
same toolbar logo in place for one second; there is no separate splash page.
Set nabla_monochrome: "true" to hide unselected control borders. This is a
presentation option, not validation of physical OLED hardware.

The brand triangle is the focus exception: inside a view it rotates 90 degrees
left over 180 ms, without a focus box. Activating it returns to the parent,
just like X/ESC. Removing focus restores its downward orientation. At the root
it remains the Home mark. The underline meets its resting lower tip.

## Shared progress line

Include progress.yaml alongside the navigation package. Call toolbar_set_progress
with value: 0 when work starts, then values 1..100 from actual progress events.
The line grows from the triangle tip and the percentage appears left of the
close-button area (also reserved at the root, where X stays hidden).
Use value: -1 to finish, hide the percentage and keep the full divider;
value: -2 hides both. Values are clamped to -2..100.
Breadcrumb space is reduced only while progress is active.
Controls have transparent backgrounds and the line is rendered above them.

toolbar_demo_load simulates 0..100 over twenty 50 ms steps. The example calls
it with the one-second logo intro. It is a visual demonstration, not a measure
of firmware or network initialization. Real callers should drive progress
with their own events and stop toolbar_demo_load before taking ownership.
The toolbar follows dark/light mode, including text and focus borders.

The header is 48 pixels high. The right-hand icon area uses Font Awesome:
red solid X in applications, and gray Wi-Fi plus Bluetooth Brands at the root.
Root icons are noninteractive placeholders, excluded from focus; they do not
indicate live connectivity. Progress reserves space before this icon area.
The line ends at the horizontal center of the rightmost icon.
