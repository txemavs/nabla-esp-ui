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
