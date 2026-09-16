# Contributor instructions

These instructions apply to the whole repository.

## Purpose and conventions
- Read README.md and simulator/README.md before making changes.
- Build reusable ESPHome + LVGL packages, not separate implementations per device.
- Use English for documentation, identifiers, and code comments. UI supports build-time Spanish and English.
- Brand: nabla.net ESP UI, black background, gray controls, white focus and labels; accent reserved for the triangle; cards may define their own colors.
- Use theme colors, nabla_font_* for text, and registered Font Awesome Free Solid icons.
- Register extra glyphs in theme/fonts.yaml and icons in theme/icons.yaml.
- Keep bundled font licenses and brand attribution.

## Architecture
- hardware/ and simulator/hardware/: physical or virtual drivers only.
- theme/: shared typography, assets, colors and icon names.
- components/: parameterized reusable visual pieces with unique instance IDs.
- navigation/: shared node hierarchy and open/back/home/move/activate actions.
- apps/: future functional modules; examples/: composition and demonstration.
- Navigation is a parent/child tree with breadcrumbs, independent of input method.
- Touch selects nodes directly; encoder bindings should use nav_move and nav_activate.
- Preserve a consistent route back and home at every depth.
- Keep installation-specific Home Assistant entity mappings outside shared components.
- IDs are resolved at compile time. Declare dependencies rather than hiding broken
  references behind null checks or proliferating unrelated stub widgets.
- Keep C++ small, bounded, and documented. Validate node IDs and parent relationships.
- User navigation is declared in examples/hello-world/navigation.yaml.
- Validate it via external_components/nabla_navigation; never hardcode node indices.
- The list currently has eight reusable row slots; document limits before extending it.

## Workflow and verification
- Work in this repository, not the earlier esphome-ui-kit fork.
- Ubuntu/WSL: source .venv/bin/activate; install requirements.txt when needed.
- Compile: esphome compile simulator/hello-world.yaml.
- Run: ./simulator/run.sh (requires graphical Linux or WSLg).
- Reuse the same UI includes for host and future device targets.
- For navigation changes, verify touch, nested entry, back, home, and window close.
- Compilation alone is not evidence of working interaction or correct visual layout.
- Add focused tests for tree invariants and navigation logic when those change.
- Do not claim ESP hardware support based on host compilation.
- Do not edit installed ESPHome files or generated .esphome outputs as a fix.
- Keep simulator timing adjustments in its hardware package.
- Run git diff --check; never commit .venv, build products, or credentials.
- Do not invent sensor readings or claim placeholder settings are functional.
- Update documentation and this file when architecture or workflows change.

## Input contract
- Normal actionable borders are medium gray; selected and pressed borders are white.
- Touch and encoder confirmation share nav_choose with 120 ms feedback.
- SDL Up/Down moves sequential focus, Enter confirms, Escape goes back, Home opens desktop.
- Test mixed mouse/keyboard navigation; keep input bindings outside shared UI.

- Treat navigation/INPUT.md as the input contract for all devices.
- Sequential movement is Up/Down, not Left/Right. Encoder rotation maps to it.
- Five-way joystick center means Confirm; optional Escape means Back.
- Keep implemented behavior and planned adapters clearly distinguished.

- Canonical input actions: UP DOWN LEFT RIGHT ENTER ESC.
- Support touch, rotary U/D/ENTER, rotary+physical ESC, five-way joystick, keyboard.
- The top-right X invokes ESC and must be reachable by sequential focus and ENTER.
- Never require physical ESC or LEFT/RIGHT without an on-screen accessible equivalent.
- Triangle and X/ESC close the current view; the keyboard Home action opens the desktop. ESC at desktop is a no-op.

## Toolbar and localization
- Read docs/ARCHITECTURE.md and locales/README.md.
- Use tr_* locale keys for new visible text; keep en/es keys identical.
- Toolbar focus order follows content, Home, ancestor breadcrumbs, X.
- Current title is bold in the theme foreground color, noninteractive and excluded from focus. The triangle and X follow the parent tree; the keyboard Home action opens desktop. Do not claim general window history yet.
- Compile both locales when changing localization or text-dependent layout.
- Declarative navigation is implemented; real sensor/data bindings remain pending.

## Persistent toolbar and appearance
- Toolbar follows the theme: black/white in dark, white/black in light.
- A continuous medium-gray bottom line separates toolbar and content; no boxed borders.
- Focus changes only the border; content colors never change with focus.
- Use the shared accent-colored logo component; preserve the original JPG as reference.
- Desktop title is translated Main menu; X is hidden there. Splash remains clean.
- Settings > Appearance switches dark/light at runtime. The preference is stored
  through ESPHome globals; persistence follows the platform save interval.
- Content focus retains the white/gray border convention.

## Component catalog
- Register reusable primitives in components/README.md with per-component docs.
- Logo uses editable equilateral SVG geometry, compiled into transparent assets.
- The triangle rotates left on interior focus and activates Back; at root it remains Home.
- Toolbar controls are transparent; the progress line must never be occluded.

- Run python3 -m unittest discover -s tests -p 'test_*.py' for catalog changes.
- Keep keys stable and titles translated; the compiler owns numeric indices.

## Brand geometry and motion
- Read docs/BRAND.md before changing the logo.
- Silver reflecting sky; equilateral, acute, substantial ring rather than thin lines.
- Keep SVG source editable and preserve original artwork.
- Respect 3/6/9 and golden-ratio preferences without forcing them into every layout.
- Intro motion is finite, one second; distinguish shaded 2D from real 3D.

- Separate intro depth-turn approximation (fixed tip) from centered planar toolbar rotation.
- SVG centroid must match the animation pivot; verify equal sides after geometry changes.

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

Toolbar progress is shared via components/toolbar/progress.yaml. Reserve space
for percentages while active, keep the line above transparent controls, and
never claim the one-second startup simulation represents actual initialization.

Toolbar right icons use Font Awesome Solid and Brands. Root Wi-Fi/Bluetooth
are display-only placeholders, never presented as live connectivity. Keep
root indicators outside the focus order and preserve X/ESC inside views.

## Current bar composition
Header and footer share components/bar/surface.yaml and nabla_bar_height.
The header has no divider or progress. Progress belongs to the gray/cyan
footer; negative progress hides it. Reserve its height in the body while
visible. Earlier header-progress/underline rules are superseded. The minimal
composition is header, body text and optional footer; tiny-screen profiles
remain planned, not validated by the 480x320 example.

The desktop footer is always present. After startup reaches 100% (held for
100 ms), progress is replaced by Soluciones Lógicas Nabla on the left and
HH:MM on the right. The shared footer_set_clock(text) action accepts clock
updates from hardware adapters; the simulator uses host time, Europe/Madrid.
Application views hide the idle desktop footer and may later supply their own.
Negative progress restores this route-specific state rather than hiding all footers.

Compact layout: shared bars are 36 px tall; launcher outer padding is 5 px
horizontally and 6 px vertically. Internal tile gaps stay 10 px. Idle footer
follows the header theme; gray/cyan is reserved for the startup progress state.

Triangle alignment: the visible left edge aligns with the 5 px content margin;
titles start at x=40. Triangle focus recolors the mark white, never draws a
button frame, and retains the interior back rotation. Clearing focus restores
the configured base/status color using LVGL state styling.

Root triangle now toggles tile/list presentation; root rotation button follows
it in sequential focus. Use runtime LVGL dimensions for layout. Verify list
scroll, returning from a child, touch after rotation, and 4x2/2x4 reflow.
Do not claim 128x64 support until a compact profile is implemented and tested.
