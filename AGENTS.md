# Contributor instructions

Applies to the entire nabla-esp-ui repository.

## Read first
- README.md and simulator/README.md for working entry points.
- docs/ARCHITECTURE.md for the implementation boundary.
- docs/platform/README.md and ROADMAP.md for planned work and completion gates.
- navigation/README.md, navigation/INPUT.md and locales/README.md for current contracts.
- docs/BRAND.md before changing brand geometry or motion.

## Purpose
- Build a reusable ESPHome/LVGL platform that works without Home Assistant.
- Keep Home Assistant, Nabla Edge, peer links and hardware as optional adapters.
- Panel YAML composes intent; reusable modules own behavior and presentation.
- No installation-specific entities or network clients inside shared widgets.
- English docs/identifiers; localized UI with matching es/en keys.
- Work in this repository, not the older esphome-ui-kit fork.

## Implemented baseline
- Host SDL, shared tree, tiles/list, scrolling, four rotations and touch/keyboard.
- Root triangle toggles tiles/list; interior triangle and X/ESC return to parent.
- Root rotation control follows the triangle in focus order; status icons do not.
- Current title is bold and not focusable; ancestors are navigable.
- UP/DOWN is sequential movement; ENTER confirms; LEFT/RIGHT are reserved
  outside contexts that define them. Physical input adapters remain pending.
- Header has no divider. Header/footer share the bar surface, now 36 px.
- Desktop footer is persistent; app footer is hidden unless an operation needs it.
- Startup progress is simulated, then brand/clock replace it.
- Triangle uses the theme foreground without a frame; interior focus rotates upward.
- Normal controls preserve their contents on focus and change the border.
- Gray idle borders, theme-foreground selected borders; monochrome option hides idle borders.
- Dark/light toolbar and footer; app tile colors remain independent.
- Use runtime logical dimensions when rendering rotated layouts.
- Settings > Connections > Wi-Fi hosts a manual SSID/password demo (wifi_demo).
- Its component owns editor focus and draft clearing; routes keep shared toolbar.
- Apply validates locally and clears the password; no Wi-Fi or persistence yet.
- Small 128x64 profile, complete forms, real data bindings and peers are planned.

## Extension rules
- Never present docs/platform/platform.proposal.yaml as accepted firmware syntax.
- Keep schema/code generation as the source of truth; reject unknown fields,
  invalid references and unsupported capability combinations.
- Preserve compile-time ID resolution; do not mask dependencies with hidden stubs.
- Extract repeated lambdas into a bounded C++ helper or external component.
- Register modules in components/README.md with parameters, states, input behavior,
  dependencies, example, resource limits and verified support status.
- Every touch flow needs a U/D/ENTER path with reachable Back/Cancel.
- Typed state must distinguish unknown/stale/unavailable; never invent readings.
- Keep credentials out of logs/examples; remote commands are allowlisted and scoped.
- UI updates belong to the UI loop; callbacks enqueue bounded state changes.
- Keep source artwork and bundled licenses; register new glyphs and icons.

## Verification
- WSL: source .venv/bin/activate; pinned requirements.txt owns dependencies.
- Compile esphome compile simulator/hello-world.yaml.
- Run ./simulator/run.sh; inspect the actual UI for visual/input changes.
- Compile es/en when changing locale-sensitive layout.
- Run python3 -m unittest discover -s tests -p 'test_*.py' for catalog changes.
- Verify root view toggle, list scroll, nested return, rotation and rotated touch.
- Documentation-only changes need link/schema-example/status checks, not firmware
  changes or an unrelated dependency upgrade.
- Never edit installed ESPHome or generated .esphome output to fix the project.
- Do not claim hardware support from host builds; publish measured target evidence.
- Run git diff --check; exclude .venv/builds/credentials from commits.
- Update current contracts rather than appending contradictory historical rules.

- Controller/target roles are independent of display size. Support the planned
  small-encoder-to-large-panel use case as well as large-keyboard-to-small-device.
  Negotiate capabilities; never infer authority or direction from screen size.

- Entering a child menu focuses its first content option and starts at the top.
  Returning may restore a content selection, but never a saved toolbar control
  when the destination has options. Empty leaves can focus Back; the Wi-Fi
  editor starts at SSID. Same-page view/rotation updates preserve current focus.
