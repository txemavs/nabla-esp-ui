# Optional web service and menu studio

Status: delivery plan, 2026-09-20. Proposed YAML below is illustrative and
NOT accepted firmware syntax. The gallery/editor is not implemented or hosted.

## Goal and current evidence

Offer optional device web menus and physical display mirrors. Separately provide
a public repository site where one menu tree can be previewed on several screen
profiles, edited and exported as ESPHome YAML.

Existing previews:
- nabla_web: offline responsive menu, tile/list toggle, camera via its existing
  MJPEG server, information and shared real Wi-Fi scanning.
- nabla_display_mirror: 128x64 monochrome draw-pass tee, canvas viewer and
  allowlisted input to the same controller as the physical encoder.
- Both have physical build/OTA/HTTP evidence. They currently conflict because
  each owns the root route. No color mirror or browser profile simulator exists.
- Omitting either component omits it. There is no unified enabled/mirror switch.
- Password migration, AP recovery and prolonged/multi-client operation remain
  unqualified. Do not convert initial tests into blanket support claims.

See the [web component](../../external_components/nabla_web/README.md) and
[mirror component](../../external_components/nabla_display_mirror/README.md).

## Device service: opt in through YAML

One owner of web_server_base must register responsive, mirror, status and input
routes. Preserve captive detection and the existing camera stream server.
Support omitted, responsive-only, mirror-only and combined compositions.
A headless device cannot enable a physical mirror; unsupported combinations
must fail validation rather than silently display an empty page.

Illustrative proposal, not runnable:

~~~yaml
nabla_web_service:
  enabled: true
  responsive:
    enabled: true
  mirror:
    enabled: true
    display_id: main_display
    allow_input: false
    max_fps: 10
~~~

Omission means off. Disabled features must exclude their handlers, pages,
capture buffers and background work at compile time, not just hide controls.
Display IDs resolve through ESPHome validation. Read-only mirroring omits its
action endpoint. Define authentication integration and capability validation
before accepting the final schema. CSRF tokens are not authentication.

Keep compatibility packages or explicit migration errors for current previews.
Do not duplicate Wi-Fi transactions or run competing root HTTP handlers.
Responsive navigation is browser-local; physical mirroring shares the device's
focus and controller. Changing a preview profile never reconfigures hardware.

## Repository studio

Target source location: studio/, with synthetic fixtures in studio/examples/.
Use the existing profile catalog rather than maintain a separate size list.
Target hosting is static GitHub Pages after verifying repository Pages setup;
do not advertise an unpublished URL. Link the published gallery from README.

The page should contain:
- A profile selector and native-size/enlarged preview.
- A menu tree beside the preview, stacked on narrow screens.
- Theme, font, tile/list and supported rotation controls.
- Encoder controls or touch interaction appropriate to the selected profile.
- Empty project, sample, import, validation and YAML download.
- A visible Demo/Live distinction; never substitute fabricated live state.

Include OLED 128x64, ST7735 160x128, T-Watch 2020 240x240 and touch 480x320
as the four initial gallery targets. Qualify each renderer before labelling its preview exact. ST7735 logical height is 128, not the informal 120.

The gallery runs without a device or Home Assistant. It must not discover
private devices, upload credentials or issue load-control commands.
Initially open live mirrors on the device's own page: HTTPS-hosted demos cannot
promise unrestricted access to HTTP LAN endpoints due to browser restrictions.

## Physical mirror implementation

Keep the existing monochrome tee. Generalize to bounded RGB565 capture for
ST7735. For LVGL, capture at the display flush boundary and assemble partial
updates into a coherent frame; do not expose half-drawn frames or render twice.

Raw RGB565 is 40,960 bytes per 160x128 frame and 307,200 bytes per 480x320
frame. At 10 FPS this is about 0.41 and 3.07 MB/s payload before HTTP overhead.
Double buffers double frame storage. These are sizing estimates, not achieved
rates. Measure available/minimum heap and PSRAM before choosing defaults.
Large panels may require dirty rectangles, reduced FPS or bounded compression.

Negotiate dimensions, pixel format, rotation, sequence and input capabilities.
Bound snapshots, request concurrency and input queues. Several viewers share a
captured frame. Pause hidden tabs and avoid capture work without viewers where
the driver hook permits. Test reconnects and slow clients. Controller effects,
backlight and display power are not represented unless explicitly captured.

## Accurate profile preview

A frame mirror cannot preview another resolution: it contains pixels rather
than menu semantics. The studio needs the tree, typed sample state, fonts and
the appropriate renderer.

First time-box a WebAssembly feasibility spike for the existing compact C++
renderer with a browser framebuffer and deterministic clock/input. Then assess
the regular renderer with the pinned LVGL version. Prove representative fonts,
navigation and animations before committing to a toolchain.

Avoid reimplementing all layout rules in JavaScript. If exact renderers cannot
be hosted economically, release a clearly labelled approximate responsive
preview plus native simulator instructions. Never silently claim pixel parity.
Exact mode requires golden-frame comparisons with native simulators at the
same revision, font, viewport and frozen animation timestamp.

## Tree editor and export contract

The existing navigation catalog is the source of truth. Generate versioned
editor schema metadata from it; do not invent editor-only firmware syntax.

Support create, rename, reorder, nest, delete and undo/redo. Start empty without
inventing content. Preserve stable keys independently of titles. Enforce the
current limits: eight children, 128 nodes, depth 16, valid unique keys and
supported actions. Revisit limits only through deliberate core-schema changes.

Actions come from a supported catalog. Keep presentation separate from bindings
and implementation code. Export:
1. A validated navigation YAML/package.
2. A selected-profile composition fragment with explicit dependencies.
3. An optional action-binding template containing clear TODOs.

Unbound actions stay visibly simulated. Do not execute imported YAML tags,
C++ lambdas or JavaScript. Start with navigation-subtree import; do not promise
lossless import of arbitrary ESPHome roots. Define a policy for preserving
opaque code before supporting it. Credentials and private bindings never enter
public fixtures. No automated firmware flash in the initial editor.

## Delivery phases and acceptance gates

### W1: consolidate optional web service
Resolve route ownership, read-only mode, authentication policy and compile-time
capabilities. Compile omitted/responsive/mirror/both fixtures; record binary
and heap differences. Preserve current OLED/camera behavior and captive routing.
Invalid configurations must fail with actionable errors.

### W2: color mirrors
Qualify ST7735 first, then LVGL. Check orientation, partial updates, complete
frames and local-input latency with zero, one and two viewers. Record runtime
heap, frame time and bandwidth; protect private network/API/OTA configuration.

### W3: gallery and profile selector
Ship generic fixtures and the static studio shell. Complete the renderer
feasibility gates; label accuracy and publish only tested profiles. Switching
profiles preserves semantic tree keys and bindings. Verify static hosting and
all assets without CDNs or live device access.

### W4: tree editor and YAML templates
Deliver empty/sample/import, schema errors, undo and export. Round-trip supported
trees; compile exported fixtures with dummy bindings for each advertised
profile/locale. Test long labels, accents, empty menus and invalid actions.

### W5: polish
Mobile and keyboard accessibility, reduced motion, screenshots, documentation
and hosting workflow. Publish support status and limitations. Confirm Back,
focus restoration and tile/list transitions across profiles.

Each phase is a reviewable PR with recorded evidence, not a large rewrite.
The immediate next step is W1, followed by ST7735 color capture. Editor work
must not block physical-device progress or require remote infrastructure.

## Key scenarios

- No web configuration: normal physical UI, no mirror buffers or capture work.
- Headless camera: offline local menu, stream and Wi-Fi setup without HA.
- Read-only mirror: no endpoint can dispatch control.
- Encoder movement appears remotely; authorized web input reaches the same UI.
- Profile switch affects only preview, never connected hardware.
- Failed Wi-Fi change restores fallbacks and Cancel clears credential drafts.
- Exported menu compiles; unbound actions cannot operate real equipment.

## First W1 increment implemented

The [optional coordinator](../../external_components/nabla_web_service/README.md) now selects responsive/mirror/both with component-presence validation. Read-only mirror routing is available by omitting on_action. The proposed enabled:false schema above remains unimplemented; omit components for compile-time exclusion. Authentication integration, AP/resource qualification and compile-time control-code elimination remain pending.
