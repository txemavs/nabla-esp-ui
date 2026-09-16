# Compact shell

Status: M1 host-verified monochrome renderer, not a hardware driver.
Consumes the same generated nabla::nodes catalog as the regular LVGL example.

Include package.yaml with a tiny/readable profile, locale, catalog and a
display adapter. The simulator entry point is simulator/compact.yaml.
Its adapter calls CompactShell::render with the display and three one-bit fonts,
then forwards touch coordinates or UP/DOWN/ENTER/ESC/Home to the model.
LEFT/RIGHT have no browse-mode action. Real encoder/joystick GPIO adapters are M3.

Parameters: compact_readable (initial one-row mode), compact_app_footer
(application footer), locale. Root footer stays visible.
State: route, selected option, scroll anchor, density, dark/light polarity.
Only the shell's triangle and content/Back rows act; titles/footer never focus.
Dependencies: generated catalog, focus.h, geometry.h, ESPHome display and fonts.
Bounds: 128 nodes and eight children from the catalog; saved focus/scroll arrays
are allocated once at catalog size. No network, file writes or stored secrets.

Tests: tests/test_profiles.py covers matching touch/sequential route traces,
scroll visibility, return/re-entry, theme leaves and geometry bounds for both
compact densities and regular orientations. See ../../profiles/README.md for
commands, measured pixel dimensions and explicit support limits.
