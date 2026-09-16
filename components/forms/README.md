# Forms and local Wi-Fi workflow

Status: M2 host implementation. These components have no radio, persistence,
Home Assistant dependency or network side effects. Hardware validation is M3.

## Declarative controls

The working example is Settings > Controls (demo), declared in
[the shared navigation YAML](../../examples/hello-world/navigation.yaml).
The same metadata drives regular/portrait LVGL and tiny/readable renderers.
This is accepted ESPHome configuration, unlike the platform proposal.

Under nabla_navigation, forms contains 1..8 fields. Each field requires a unique
key, label, type and initial value:
- number: int32 min/max, positive step (default 1), aligned bounds/initial.
- choice: 1..8 distinct option labels, zero-based initial index.
- toggle: boolean initial value.

Unknown fields, duplicate keys/options, invalid types and out-of-range defaults
fail configuration. Labels/options are bounded to 96 UTF-8 bytes.
Use localized substitutions in labels/options. The example defines brightness,
refresh interval, operating mode and notifications. They are simulated settings;
changing brightness does not control the host monitor or a device backlight.

Add action: forms_demo to a leaf to open the configured form. The current schema
has one form definition per firmware composition; multiple independent named
forms and arbitrary data bindings are future extensions, not accepted syntax.

## Transaction and input contract

external_components/nabla_navigation/forms.h contains the presentation-independent
Field, Session and Controller primitives. The schema emits immutable descriptors.
Session owns at most eight retained integers and eight draft integers.

- Entering copies retained values into a draft and focuses the first field.
- Moving focus never commits. A toggle changes only the form draft.
- A number opens Minus, Plus, Accept and Cancel. Up/Down/Enter reaches all four;
  Left/Right optionally adjusts the value, clamped to its declared range.
- A choice opens its options followed by Cancel. Up/Down browses; Enter accepts
  into the form draft. Left/Right optionally browses choices.
- Field Cancel/ESC discards that field edit and restores its original focus.
- Save opens a confirmation with Cancel initially focused. Confirm commits once.
  Repeated commit calls are rejected after the transaction closes.
- Form Cancel, Home or departing through an ancestor discards uncommitted data.
  Dialog Cancel returns to Save without discarding the form draft.
- Re-entering shows the last confirmed values for this process. Restart resets
  them to YAML defaults; there is no storage or production settings backend.

RegularForm renders up to ten reusable LVGL row objects in a scroll container.
CompactShell renders the same Controller directly, with three rows or one large
row. Focus always scrolls into view; long focused labels scroll horizontally.
The confirmation, validation error and saved state use the same controller and
input contract. No modal loop blocks the UI.

## Text/password and Wi-Fi

wifi_flow.h provides bounded UTF-8 text drafts and a deterministic asynchronous
Wi-Fi workflow: editing, scanning, results, scan error, connecting, success,
failure. CompactWifi uses field drafts; the regular form uses native textareas
as its draft. Neither editor applies a real connection.

SSID validation accepts 1..32 UTF-8 bytes; a protected password requires 8..63
UTF-8 bytes. Open networks ignore the password. Raw 64-digit PSKs, enterprise
authentication, arbitrary binary SSIDs and general IMEs are outside this demo.
Append/delete preserves complete UTF-8 code points; malformed sequences and
embedded NUL are rejected. Supported interactive text repertoire: ASCII and
Spanish accents, including uppercase accents, ñ and ü.

Passwords are always masked, including the last entered character. Accepted
connection attempts immediately clear the password. Leaving clears both fields.
No entered credentials are logged, persisted or placed in result labels.
Logical clearing is not a guarantee of forensic memory erasure.

## Scan and operation fixtures

Scan completes after 700 ms and connection after 1000 ms when the UI ticks.
All four profiles offer normal scan, empty scan and scan error.
Regular Scan first opens a scenario picker; compact has explicit fixture rows.

Normal fixtures include protected Nabla Demo, open Invitados Demo, protected
Demo sin servicio, accented Café Demo, a 32-byte SSID and an open Nabla Demo.
Duplicate SSID plus security entries collapse to the strongest RSSI while
preserving first-seen order. Open and protected names stay separate, labeled
[O] and [P]. Seven advertisements yield six visible results, capped at eight.
Do not treat these labels or RSSI as device identity or authorization.

Demo sin servicio fails after a valid protected submission. Other fixtures
succeed with valid-length dummy input. Protected retry requires password re-entry.
Open success can be tested by selecting Invitados Demo.

Busy operations reject another scan/submit. Completed success cannot be
resubmitted until editing starts again. accepted counts accepted mock submissions;
revision invalidates a cancelled operation locally. A cancelled tick cannot
complete the previous request; wraparound of the millisecond clock is tested.
The pending result is captured when submitting, so later draft edits cannot
change an in-flight outcome.

Regular uses five reusable result rows and scrolls focus to expose remaining
results/Back. Compact scrolls its three rows, including long selected SSIDs.
Every scan/busy/error state has a reachable Back or Cancel without physical ESC.

## Keyboard and component composition

[package.yaml](package.yaml) is the reusable LVGL form entry point.
[regular.h](regular.h) depends on generated metadata and LVGL; the compact renderer
uses the same model without LVGL. Theme/focus colors are supplied by the shell.

[latin_keyboard.h](latin_keyboard.h) installs optional native LVGL maps with
lowercase, uppercase, digits, ASCII punctuation and Spanish accents. It retains
LVGL editing/cursor/Ready/Cancel behavior. The Wi-Fi package enables it through
nabla_keyboard_latin: "true"; set "false" to retain the stock keyboard.
The shared font combines DejaVu text with five bundled Font Awesome symbols.

Compact text input uses an encoder character selector: Done, Delete, Cancel,
palette and characters. Up/Down/Enter reaches every action. A field opens on
Done, which preserves its existing text if no edit is made. Space is shown
as an underscore. Touch activates the center item and uses the footer halves
to move; header tap cancels. Password drafts always display a mask.

## Extending beyond the mock

Real networking must remain an adapter; do not put radio calls in renderers.
The M3 adapter must snapshot a validated request before the draft is cleared,
tag asynchronous results with an operation identity and ignore cancelled/older
results. Marshal callbacks onto the UI loop. Bound scan results and publish
explicit busy, empty, unavailable, failure and success states.

A local accepted counter is not a distributed deduplication protocol.
Real credential storage, timeout/rollback and network recovery require M3
hardware evidence. Transport authorization, pairing and peer operations belong
to later milestones. Preserve the same UI transaction and cancellation contract.

See [M2 verification](../../docs/platform/M2-VERIFICATION.md) for tests and limits.
