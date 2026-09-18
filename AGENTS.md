# Contributor instructions

Applies to the entire nabla-esp-ui repository.

## Read first
- README.md and simulator/README.md for working entry points.
- docs/ARCHITECTURE.md for the implementation boundary.
- docs/platform/README.md and docs/platform/ROADMAP.md for planned work and completion gates.
- navigation/README.md, navigation/INPUT.md and locales/README.md for current contracts.
- docs/BRAND.md before changing brand geometry or motion.

## Purpose
- Build a reusable ESPHome/LVGL platform that works without Home Assistant.
- Keep Home Assistant, Nabla Edge, peer links and hardware as optional adapters.
- Panel YAML owns its navigation tree/forms and installation configuration; reusable modules own behavior and presentation.
- Builder compositions import packages/regular.yaml or packages/compact.yaml, not an example menu.
- No installation-specific entities or network clients inside shared widgets.
- English docs/identifiers; localized UI with matching es/en keys.
- Work in this repository, not the older esphome-ui-kit fork.

## Implemented baseline
- Host SDL, shared tree, tiles/list, scrolling, four rotations and touch/keyboard.
- Root triangle toggles tiles/list; interior triangle and X/ESC return to parent.
- Root rotation control follows the triangle in focus order; status icons do not.
- Current title is bold and not focusable; ancestors are navigable.
- UP/DOWN is sequential movement; ENTER confirms; LEFT/RIGHT are reserved
  outside contexts that define them; M2 numbers/choices define optional adjustments.
  NodeMCU-32S GPIO encoder/push/K0 mapping is deployed; joystick and generalized input adapters remain pending.
- Regular header/footer share a 36 px bar surface; compact bar geometry belongs to its profile.
- Desktop footer is persistent; app footer is hidden unless an operation needs it.
- Startup opens the menu directly; progress and logo animations are opt-in for operations.
- The example Settings starts with Information; device YAML may expose Information at root.
  Read-only info fields share live model/network/version/uptime.
- Triangle uses the theme foreground without a frame; interior focus rotates upward.
- With borders enabled, normal controls preserve contents and change the focus border.
- Appearance can disable borders; focus then inverts the selected control instead.
- Theme-relative idle borders (#404040 dark / #BFBFBF light), foreground selected borders.
- Monochrome hides idle borders; the separate Borders option removes focus borders too.
- Dark/light toolbar and footer; app tile colors remain independent.
- Use runtime logical dimensions when rendering rotated layouts.
- Settings > Connections > Wi-Fi hosts a manual SSID/password demo (wifi_demo).
- Its component owns editor focus and draft clearing; routes keep shared toolbar.
- The default form is a mock; optional nabla_wifi enables real scan and persistent primary Wi-Fi.
- Preserve declared fallbacks, cancel/timeout rollback and confirmed primary removal.
- Never wire wifi.configure directly: it replaces the active network list.
- Password show time stays zero; never reveal even the last typed character.
- M2 Save commits once after confirmation; accepting a field changes only its draft.
- Keep focus/rollback rules in the shared controller, not separate renderer callbacks.
- Wi-Fi scenarios include bounded deduplication, long names, empty/error, cancel/retry.
- M1 tiny/readable profiles render the same tree at real 128x64, monochrome.
- profiles/README.md documents geometry, launch commands and host-only evidence.
- M2 Wi-Fi forms work in compact and regular with a shared simulated scan/connect flow.
- M2 generic number/choice/toggle forms use validated YAML and shared transactions.
- devices/jc3248w535cn.yaml reuses the UI through hardware/jc3248w535cn.yaml.
- Initial physical LCD/touch navigation passed on 2026-09-16; M3 is not complete.
- The board starts at nabla_initial_rotation=90; SDL defaults to 0.
- On the JC3248W535CN, LVGL owns rotation; do not also rotate native display/touch coordinates.
  On the ST7735 compact target, the display driver owns rotation=270 (logical 160x128).
- Use ROM (--no-stub) USB flashing for the first unit; preserve its private backup.
- NodeMCU-32S/ST7735 uses tft160: no LVGL/PSRAM requirement, encoder movement,
  push activation and K0 Back. See hardware/nodemcu-32s-st7735.md for wiring/evidence.
- Optional compact-shell/color.yaml adds 2x2 color tiles and a one-icon view;
  nested lists and information details remain scrollable with the encoder.
- MQTT regular/compact editors share transport.yaml and its freshness/confirmation
  rules. Compact brightness is a draft until Apply; Back cancels.
- Real network information and MQTT state reception are deployed. Generalized
  input adapters, OLED hardware validation and peer cooperation remain pending.

## Extension rules
- New examples and documentation must use generic names (Site A, Site B, generic
  rooms/entities). Do not introduce real installation names or personal site references.
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
- Follow docs/PRIVATE-INSTALLATIONS.md: site YAML and bindings stay outside this repo.
- Never publish real entities, camera URLs, network inventory, firmware or backups.
- Use nabla_resource_root for font/image paths; YAML !include paths stay source-relative.
  GitHub consumers should explicitly set nabla_resource_root in root substitutions
  to https://raw.githubusercontent.com/txemavs/nabla-esp-ui/${nabla_ui_ref}.
  A sibling package default can override the entry-point URL; local assets can
  hide this error. Do not add resource-root defaults to modules that do not use assets.
- Information uses semantic info keys (model/wifi/ip/version/uptime), never route
  indexes as offsets into nabla_info arrays.
- Compact profile dimensions/font sizes belong in profiles, not overriding shell
  defaults. Keep SDL key initialization out of hardware-independent packages.
- nabla_runtime exports flat header basenames: use unique names across modules
  (mqtt_model.h versus compact model.h), and pass Path objects to include_file.
- Optional captive_portal is a pinned Git external component, separate from UI
  versioning. Preserve wifi/ap, API, OTA and fallbacks when migrating its source.
  Keep upstream license notices and inspect decoded gzip/Brotli before publication.
- Export helper headers through nabla_runtime, never consumer-relative source paths.
- Validate independent GitHub consumers; packages/regular.yaml pins components and
  assets to the same nabla_ui_ref as its package import. Keep menus device-owned.
- UI updates belong to the UI loop; callbacks enqueue bounded state changes.
- Keep source artwork and bundled licenses; register new glyphs and icons.

## Verification
- WSL: source .venv/bin/activate; pinned requirements.txt owns dependencies.
- Compile esphome compile simulator/hello-world.yaml.
- Run ./simulator/run.sh; inspect the actual UI for visual/input changes.
- Compile es/en when changing locale-sensitive layout.
- For profile changes compile simulator/compact.yaml, readable.yaml, tft160.yaml and portrait.yaml;
  inspect native-resolution output and preserve both compact densities.
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
