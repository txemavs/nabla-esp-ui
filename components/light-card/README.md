# Light card (experimental regular renderer)
Include widget.yaml to export light_card.h. The C++ card function accepts the LVGL button/label, title, State, Words, theme, focus and dimensions. State carries on, brightness (0–100), dimmable and available. The caller owns authoritative state and command delivery; no network dependency is included.
Editor.open accepts a title, state, localized Words, font, theme and commit callback. Changes remain a draft until Apply; Cancel/ESC discards them. Route U/D to move, Enter to activate, ESC to close while active. Enter on the slider toggles editing; U/D then adjusts in steps of five. Touch uses the native slider. Three buttons toggle power, apply and cancel.
Resource bounds: one editor overlay at a time, fixed three buttons, no history. Caller chooses up to eight cards in the preview. Regular color host prototype only; compact, monochrome, hardware, and production schema integration remain pending.

## Small-display adaptation
The 128x64 profiles use a list, not a scaled 4x2 grid: tiny shows three rows,
readable one large selected row. A light must expose name and explicit on/off,
unavailable and percentage text; RGB cannot carry state on monochrome.
Brightness editing should reuse the compact numeric form (0-100, step 5),
with Apply/Cancel and U/D/Enter/ESC. Optional icons are omitted where they
compete with text. Logs need one bounded message at a time, not the desktop
paragraph. The existing compact menu/forms implement these primitives, but
this new light editor is still regular-only; the compact light adapter and
its state/command mapping have not yet been implemented or hardware-tested.

## Optional grid integration

panel.yaml reuses the eight navigation rows as a 4x2 or 2x4 light grid.
Import it after the regular shell. Configure nabla_light_slot_base to return the
first state slot for the current route, or -1 for a normal page.
nabla_light_state is a State expression with base and slot in scope;
nabla_light_pending returns whether that slot has an outstanding command.
nabla_light_footer supplies the status text. The package never creates fake
device state or connects to a broker; those belong to the private adapter.

The package delegates U/D/ENTER and Back to the brightness editor while open.
When composing with another view, explicitly combine nabla_view_refresh and
nabla_back_intercept in the root YAML; do not let package order silently replace
another application's hook. For example, call light_panel_render followed by
camera_render, and test editor.active() before camera_fullscreen in Back.

The recovered private Control composition still uses simulated acknowledgements
and displays DEMO | MQTT simulado. Real MQTT binding is a separate integration.

The grid owns its row labels while active (nav_custom_rows). The shell's
periodic information refresh must not replace localized card text with command
state strings. Leaving the grid restores ordinary row refresh.
