# Architecture and module contracts

Status: proposed target architecture. See README.md for current implementation.

## Layers and ownership

A panel manifest selects capabilities, profiles, navigation, applications and
bindings. An ESPHome external component validates the manifest at build time
and emits C++ descriptors and concrete IDs. Native includes remain the way to
assemble hardware and ordinary ESPHome entities; no mandatory separate CLI
generator is introduced.

The runtime has five small parts:
- Navigation/focus: stable routes, activation and edit context.
- State/actions: typed values, command execution and operation progress.
- Presentation: component instances and the chosen display profile.
- Applications: reusable behavior such as Wi-Fi settings or light control.
- Adapters: ESPHome entities, Home Assistant, MQTT, Edge or peer transport.

Only adapters know network protocols and entity identifiers. Widgets consume
typed state and dispatch semantic actions. Local C++ uses ordinary classes,
callbacks and bounded containers; ESPHome's compile-time ID rules still apply.
No runtime reflection, downloadable plugins or per-widget network clients.

Proposed package ownership:
core/ for state/navigation contracts, components/ for visuals, apps/ for
behaviors, profiles/ for presentation policies, adapters/ for data/transports,
hardware/ for board wiring, locales/ for strings, examples/ for working panels.
Migrate incrementally from navigation/ and the current external component;
do not move every directory before the first complete application works.

## Compile-time contract

Every module declares:
- Stable module ID, module version and supported schema version.
- Required capabilities and optional features with explicit fallback.
- Typed configuration, defaults, limits and unknown-field rejection.
- State inputs, action outputs, ownership and read/write permissions.
- Locale keys, icon/font/image dependencies and required glyphs.
- Supported input/display profiles; unsupported combinations are errors.
- Per-instance IDs, bounded RAM/flash costs and lifecycle hooks.
- Runnable example, checks performed and hardware status.

Composition produces a capability report and resource summary before compilation.
Missing action/binding references fail with a YAML path and suggested correction.
Disabling a module removes its bindings and allocations; do not create hidden
stub widgets to mask unrelated dependencies.

YAML configures intent. Small local lambdas are the escape hatch; repeated or
protocol-heavy logic moves into a named module. Reuse native ESPHome actions
instead of inventing wrappers for every operation. Secrets remain in !secret
or runtime commissioning storage, never in published examples.

Draft fields in platform.proposal.yaml are design vocabulary, not current syntax.
Before accepting them, implement the ESPHome validation schema, positive/negative
fixtures, documented migrations and one working example. Generate editor schema
and agent-readable module metadata from the same definitions to avoid drift.

## State model

A value is more than its payload:
type, value, unit, availability, source, monotonic observation age, revision,
and optional validation/error code. Availability is unknown, available, stale,
unavailable or unsupported. A missing temperature must never become zero.

An action declares arguments, capability requirements, permission, idempotency,
deadline and cancellation behavior. Execution returns a request/operation ID
and progresses through queued, running, succeeded, failed or cancelled.
Acknowledged means received, not completed. Commands with side effects are not
blindly retried. Optimistic presentation must show pending state until confirmed.

Operations expose optional total/completed or a percentage. Without a measurable
total, show activity and elapsed time, not invented percentages. Errors retain
a stable code, localized explanation and explicit Retry/Cancel where possible.
One central operation registry owns progress; application switching does not
erase an operation. The global footer shows the selected operation and count of
others; the operation page exposes the rest. Limit concurrent operations by profile.

Updates run on the UI thread. Adapters enqueue bounded/coalesced state changes;
BLE callbacks and high-frequency CAN callbacks never mutate LVGL directly.
Drop superseded telemetry, not release events or command results. No blocking
scan, wait-for-network loop or synchronous remote request in a render callback.

## Navigation and lifetime

Keep stable route keys independent of label, locale, grid position and numeric ID.
Current parent-tree behavior remains the default:
root triangle changes view; interior triangle and X/ESC go to the parent.
Home is a separate semantic action, available through the input adapter.

For cross-application routes, introduce an explicit bounded navigation stack
with route, arguments, focus key and scroll anchor. Breadcrumbs describe ancestry;
Back restores the actual previous view when stack navigation is explicitly used.
Do not silently change all existing tree routes to history semantics.

Views support create, enter, suspend, resume and dispose. Subscribe on enter;
unsubscribe on dispose; pause invisible animations and coalesce hidden updates.
Per-profile cache limits bound the number of retained views. An operation may
outlive a view; its callbacks resolve state IDs, not deleted widget pointers.
Editing state belongs to a form session, not a reused LVGL row.

Reusable visual slots: header leading/title/trailing, body, footer
leading/content/trailing, overlays. Header and footer share surface metrics.
Desktop idle footer is permanent; applications choose inherit, own or hidden.
Modal dialogs capture focus and restore it on close. At most one editor overlay
per small profile. Empty lists, disabled actions and errors still expose Back.

## Portability and offline operation

A local-only build omits Home Assistant and any compulsory broker. The same light
screen binds to a local ESPHome light, an HA entity or an Edge endpoint through
adapters. The application sees brightness/on-state and a typed set action.

Separate network interfaces from services: Wi-Fi associated, IP acquired,
gateway accessible, HA connected and Edge ready are different states.
Review ESPHome Wi-Fi/API reboot_timeout policies for standalone builds so an
unavailable optional service does not cause periodic UI reboots. Recovery still
needs a bounded reconnect policy and a deliberate watchdog strategy.
See the [ESPHome connectivity notes](SOURCES.md).

No common RAM assumption for all ESP chips. Fonts, buffers, TLS/BLE stacks and
images dominate different targets. At compile time choose a build profile;
runtime view/locale changes can use only assets compiled into that build.
A second native monochrome renderer is a gated option if measured LVGL overhead
is excessive; retain the same app/focus/state contracts and avoid a forked UI.

## Stability and extensibility

Version panel schema, module API and peer wire protocol independently.
Semver releases have pinned tested dependencies, migration notes and deprecation
windows. Prefer optional additive fields with bounded defaults. Unsupported major
versions fail early; never interpret an unknown action as another action.

Core v1 is done when the reference applications, profiles and conformance checks
pass and another contributor can add a module without changing core internals.
Maintenance, upstream updates and new modules continue after that milestone.
