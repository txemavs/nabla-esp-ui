# Shared UI contract and remaining work

This contract keeps Nabla components recognizable and usable across supported
devices. It strengthens the existing library contract; it does not replace the
working implementation or restart the platform roadmap.

## Ownership

Device YAML declares hardware, menu content, application choices, data bindings,
actions and initial system preferences. Nabla owns standard presentation and
interaction through shared components, profiles and navigation controllers.
Consumers should not need per-screen drawing code to create a standard menu.

System applications use the same shared controls and appearance rules as menus.
A custom application may draw its own content, including a game. Embedded Nabla
widgets, system overlays and the documented exit path still obey the system
contract. Custom drawing does not authorize restyling system controls.

## Global appearance

- Resolve supported appearance preferences centrally. A change applies to all
  Nabla pieces in the active device composition, including system applications,
  forms, overlays and widgets embedded in custom content.
- Fonts, spacing, control dimensions, borders and focus use shared semantic
  roles. Different normal/selected/disabled states are valid shared variants;
  independent per-screen visual policies are not.
- Reuse or extend the owning component when a variant is missing. Do not copy a
  widget and assign a local font or border to solve one application's problem.
- The target contract includes global icon visibility and coordinated interface
  size choices. Icons off must preserve meaningful labels and usable controls.
  Size changes must coordinate typography and geometry, keeping every action
  reachable. These are acceptance requirements, not claims that all renderers
  already expose a complete global icons/size API.
- Profiles own minimum legibility and supported geometry. Small screens reflow,
  scroll or paginate instead of shrinking text below that minimum. Different
  hardware presentations preserve common meaning, state and navigation rules.
- Hardware adapters map available inputs to common actions. Every interactive
  flow needs reachable activation, Back and Cancel. A no-input device needs an
  explicitly supported passive/remote mode, not unreachable local controls.
- Preserve existing preference persistence where implemented. Build-time
  substitutions are not live settings; live changes must also reach components
  created after the change.

## Already implemented

The published library already provides declarative navigation trees and form
schemas, regular LVGL and compact display renderers, shared focus/navigation,
profiles, runtime appearance controls (dark/light, font family, border/focus
behavior), and compact readable/icon/list presentations. Device information,
light adapters, camera previews/cache and optional Wi-Fi provisioning also exist.

Do not describe those features as a future rewrite. Consult the
[library contract](LIBRARY-CONTRACT-v0.1.md), [profiles](../profiles/README.md),
[components](../components/README.md) and [roadmap](platform/ROADMAP.md) for their
limits. Runtime appearance controls do not prove that every widget already
honors every setting. A view-mode toggle is not a universal icon-visibility API.

JC3248W535CN touch and NodeMCU-32S/ST7735 encoder deployments have documented
physical evidence. Tiny/readable OLED profiles have host evidence; physical OLED
validation and full M3 recovery/usability/soak gates remain open. T-Watch work is
separate branch work until merged and verified; T-Wristband is a separate device
and requires exact revision/driver/input validation.

## Implementation follow-up

1. **Inventory actual coverage.** Trace appearance settings through both
   renderers, forms, controls and overlays. Record which already comply and
   specific exceptions. Preserve working hardware behavior as the baseline.
2. **Close shared gaps.** Extend shared appearance state and semantic geometry
   for consistent icon visibility and supported size choices. Validate names and
   schema against real ESPHome builds before documenting new public YAML keys.
   Preserve stored preferences and migration behavior.
3. **Migrate exceptions through primitives.** Update owners and consumers
   together. Avoid local overrides and parallel implementations of existing
   menus, Wi-Fi flows or settings.
4. **Verify supported combinations.** Cover dark/light, border modes, font
   choices, icon visibility and sizes where implemented; long labels, empty or
   unavailable data, native-resolution output, rotation and Back/Cancel paths.
   Include actual touch/encoder devices and record untested combinations.
5. **Publish evidence and examples.** Maintain the display/test catalog and
   existing M3 gates. Keep private deployment YAML and credentials outside the
   public repository; publish generic compositions and reproducible evidence.

These tasks extend [M1–M3 and M8](platform/ROADMAP.md). They do not replace those
milestones or claim automatic compatibility with every screen resolution.

## Review checklist

- Identify the shared component/profile that owns the change.
- State which preferences and device profiles are affected.
- Check all affected Nabla pieces, including application views and overlays.
- Record relevant schema, compile, visual and physical checks and their limits.
- Keep consumer menus declarative and preserve existing network/API/OTA blocks.
- Write project documentation, comments and help in English; preserve explicit
  localized UI resources and user-owned names/bindings.
