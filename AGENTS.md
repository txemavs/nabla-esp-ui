# Contributor instructions

These instructions apply to the whole repository.

## Purpose and conventions
- Read README.md and simulator/README.md before making changes.
- Build reusable ESPHome + LVGL packages, not separate implementations per device.
- Use English for documentation, identifiers, and code comments. UI supports build-time Spanish and English.
- Brand: nabla.net ESP UI, black background, cyan accents, original triangle logo.
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
- Current tree.h and launcher.yaml are an initial static catalog, not the final public API.
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
- Normal actionable borders are cyan; selected and pressed borders are white.
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
- Triangle means desktop; X/ESC means close current view. ESC at desktop is a no-op.

## Toolbar and localization
- Read docs/ARCHITECTURE.md and locales/README.md.
- Use tr_* locale keys for new visible text; keep en/es keys identical.
- Toolbar focus order follows content, Home, breadcrumbs, X.
- Home opens desktop; X follows the parent tree. Do not claim general window history yet.
- Compile both locales when changing localization or text-dependent layout.
- Declarative navigation YAML is pending; do not describe the current static catalog as final.

## Persistent toolbar and appearance
- Toolbar remains black in both light and dark content modes.
- A continuous cyan bottom line separates toolbar and content; no boxed borders.
- Focused toolbar sections invert to white background and black text.
- Use the shared cyan line logo component; preserve the original JPG as reference.
- Desktop title is translated Main menu; X is hidden there. Splash remains clean.
- Settings > Appearance switches dark/light at runtime. The preference is stored
  through ESPHome globals; persistence follows the platform save interval.
- Content focus retains the white/cyan border convention.

## Component catalog
- Register reusable primitives in components/README.md with per-component docs.
- Logo is vector-like LVGL line geometry, shared by splash and toolbar.
- Focused interior Home points left; its action still opens desktop.
- Keep the toolbar black; logo focus does not invert its background.
