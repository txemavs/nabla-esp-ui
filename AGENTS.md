# Contributor instructions

Applies to the entire nabla-esp-ui repository.

## Language and file explanations
- Write project documentation, code comments, help text and new identifiers in
  English. Preserve explicit es/en UI localization and user-owned names/bindings.
  Conversations and explicitly Spanish issue/discussion threads may use Spanish.
- Start each owned source/configuration file with a brief English comment saying
  what it does and why it exists. Keep useful existing headers and explain
  non-obvious dependencies. Preserve shebangs, encoding declarations and licenses.
- Do not edit generated assets or verbatim upstream source just to add a header;
  document those files, and comment-free formats such as JSON, in a nearby README.
- Keep headers current and distinguish implemented behavior from planned work.

## Shared presentation rules
- Follow docs/UI-CONSISTENCY.md. Menus declare content/actions; Nabla owns standard
  presentation, navigation and adaptation through shared components/profiles.
- Global appearance applies to every Nabla piece, including system applications,
  overlays and Nabla controls embedded in custom applications.
- Extend shared primitives or semantic variants; do not invent per-screen fonts,
  border/spacing policies, icon visibility or physical input gestures.
- Preserve profile legibility floors and reachable Back/Cancel; reflow, scroll
  or paginate instead of shrinking text to fit.
- Existing local exceptions are coverage gaps to fix centrally, not precedent.
- Preserve the tested touch/encoder behavior and existing appearance controls.
  Do not describe implemented menus, renderers or Wi-Fi adapters as future work.
- Distinguish runtime settings from build-time defaults and view modes from a
  universal icon/size API. Verify coverage rather than claiming it from a demo.

## Read first
- README.md and simulator/README.md for working entry points.
- docs/ARCHITECTURE.md for the implementation boundary.
- docs/UI-CONSISTENCY.md for shared appearance and docs/DISPLAY-CATALOG.md for hardware evidence.
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

## Shell architecture (content vs chrome)
- nabla-esp-ui is a small OS: you navigate a structure; the shell offers services.
- Apps supply content (list rows, labels, actions). The shell owns all chrome/style.
- Applications must NOT draw their own selection borders, play markers, nabla glyphs,
  or Normal vs Alto contraste focus chrome. They present content; the shell applies
  global appearance settings via shared primitives (RowChrome, NablaChrome in render.h).
- To add a new list screen: wire content into the shell and inherit chrome automatically.
- Wi-Fi, forms, and menu lists all use the same chrome primitives—no duplication.
- Apps request semantic inputs only (e.g. "fill this text/password field"). They do NOT
  choose the keyboard UI. The shell adapts input presentation by profile and hardware:
  touch-capable/large displays get a full on-screen keyboard; encoder-only Tiny gets
  whatever compact keyboard fits the space. Form asks for a string; shell owns how it's typed.

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
  input adapters, broader OLED qualification and peer cooperation remain pending.

## Extension rules
- New list screens use shell chrome primitives; do not duplicate focus/selection drawing.
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

## T-Call integration experience

- The T-Call + external SSD1309 assembly has a public device root and hardware
  record; read hardware/tcall-ssd1309.md before adapting or flashing it.
- Check the running API project version before trusting Device Builder YAML:
  the saved configuration can be older than the installed firmware.
- Compact forms must reuse the shared logo and selection policy, including
  encoder-reachable Back and masked password drafts.
- Host clean Wi-Fi presentation does not imply real radio connectivity.
  Preserve network/API/OTA settings and private MQTT bindings during migration.
- Distinguish owner-confirmed appearance/operation from unperformed recovery,
  individual load-control and soak tests. Do not turn approval into a blanket
  hardware qualification claim.

## Web and Home Assistant ownership

- Firmware web/mirror components live here; Nabla Control lives in
  https://github.com/txemavs/nabla-hacs. Do not copy its predecessor integration
  back into this repository. Keep the HTTP contract compatible across projects.
- Read docs/platform/DISPLAY-MIRROR-CONTRACT.md before changing transport.
- studio/ is a live equipment selector, not a cross-profile simulator/editor.
- Preserve mono1 and RGB332 consumers, optional input and root coordination.
- Read-only tokens are not authentication; document trusted-LAN scope.
- Record compiled, captured and owner-confirmed hardware evidence separately.
