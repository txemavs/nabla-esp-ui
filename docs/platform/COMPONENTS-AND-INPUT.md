# Components, profiles and input

Status: proposed catalog and interaction contract; current primitives are listed
in ../../components/README.md. A component is not supported until its module
example and profile checks exist.

## Component families

Build each family on existing LVGL/ESPHome primitives where possible:
- Shell: application surface, header/footer, title, breadcrumb, icon slots,
  safe content region, scroll container, separator, stack and responsive grid.
- Navigation: launcher tiles/list, nested menu, tabs/segments, drawer as an
  optional presentation, paged lists, selectable rows and search/filter.
- Display: label/value/unit, icon/image, badge, status, avatar/thumbnail,
  description, empty/error state and last-updated indicator.
- Forms: text/password, numeric field/stepper, toggle, checkbox, radio/select,
  range slider, date/time picker and multi-line text where the profile allows.
- Feedback: determinate/indeterminate progress, busy indicator, inline error,
  nonblocking notification, confirmation, modal and action menu.
- Data: sensor cards, bounded history chart, gauge and simple table/list detail.
- Media: images and bounded animation first; camera preview, audio/video and
  large chart histories are capability-gated modules, potentially Edge-backed.
- Tools: on-screen keyboard, character wheel, keypad, device picker,
  pairing prompt, Wi-Fi picker and remote-control pad.

Do not pursue desktop/web feature parity. Infinite scrolling becomes bounded
paging; large tables become summary/detail; swipe-only actions have visible
alternatives; drag reordering also has Move up/down commands. Rich media is not
a core prerequisite. Avoid expensive shadows/transparency on constrained builds.

Each component documents normal, focused, pressed, disabled, loading, invalid,
empty and disconnected states. It exposes accessible text even if the visible
presentation is an icon. Focus is never signaled only by hue. Application colors
are independent from focus; the triangle has its established white/rotation
exception. On light or monochrome profiles, use sufficient contrast rather
than mechanically preserving invisible white outlines.

## Profiles are explicit capabilities

Selection uses logical resolution, color depth, input devices, available memory,
font metrics and physical readability preference. Diagonal inches alone are not
enough. Profiles are selected at build time with optional supported view changes
at runtime. Orientation swaps the viewport and reflows it.

Regular color: existing 3–4 inch target class, tiles or list, shared shell,
touch keyboard and optional sequential control. Readable variant favors fewer
larger targets, wider labels and larger text over retaining eight visible tiles.
No user-critical text automatically shrinks below the chosen readable minimum.

Large OLED: determine actual resolution and monochrome/grayscale support first.
Use a roomy list or summary/detail, minimal ornament and a persistent selection
marker. Offer fewer items with larger text. Do not equate OLED with 128x64.

Tiny 128x64: start with a 12 px header, three 12 px rows and a 12 px footer,
leaving four pixels for gaps. Prototype 10 px glyphs inside those rows.
A readable alternative uses a 16 px title, one 24 px item/detail region and
a 16 px action/footer area, leaving eight pixels of spacing.
Both are proposed layouts to validate physically, not existing presets.
Long titles use a detail page or deliberate marquee on focus; never silently
remove the only distinction between two choices. Branding shortens or hides.

Text-only/two-line: application declares a primary value and short status.
The profile may omit header/footer or show two text bands; navigation returns
when input arrives. Do not require the desktop footer on a display too small
to render it. This is a profile exception, not a change to the regular desktop.

Monochrome: no gray border in idle, selected row/outline uses available contrast.
For very short rows prefer a side marker or inversion over a space-consuming box.
Accent/error distinctions gain a symbol or word. Static OLED elements may dim or
shift while idle, but never move while the user is editing or navigating.

## Canonical input

All adapters produce UP, DOWN, LEFT, RIGHT, ENTER and ESC with press/release,
timestamps and an optional bounded repeat. Hardware adapters own debounce;
UI code owns meaning. Touch selects and activates a target using the same action.
A mouse behaves as touch; hover is never required.

Browse mode: UP/DOWN traverse deterministic order and scroll into view.
ENTER activates. ESC returns, or closes the active overlay first.
LEFT/RIGHT may navigate a two-dimensional keyboard or edit a value; they are
never required for the basic path because encoder-only devices provide U/D/E.
The five-way joystick maps directly; its center is ENTER.

Edit mode: ENTER on a field opens its editor. UP/DOWN adjust the value, cycle
a character group or move through selectable editor controls. Save, Cancel,
Backspace, case/symbol switch and cursor positioning must all be selectable
without LEFT/RIGHT. ESC cancels the editor; unsaved multi-field forms may ask
whether to discard. ENTER must not both enter a field and submit its form.

Sequential order is declared or derived from semantic content, not screen
coordinates. Disabled targets are skipped, with an accessible reason elsewhere.
Header/footer status labels are not focusable. Focus is restored by stable key
after list refresh, sorting, rotation or return. If an item disappears, select
the nearest surviving item and never activate a replacement accidentally.

Remote input uses the same actions with a session/source ID; local ESC can exit
remote mode. A missing release, disconnect or expired session releases held
actions. Acceleration/repeat is bounded and never used for destructive commands.
Long-press/double-press are optional shortcuts, never the only way out.

## Text, password and keyboards

Create one bounded editor model: UTF-8 text, cursor/selection, maximum bytes and
characters, allowed input, draft value, validation, dirty state, Save and Cancel.
The UI representation is replaceable: touch keyboard, hardware keyboard,
joystick keyboard grid, rotary character picker or assisted input from a peer.

The existing ESPHome keyboard and textarea are the starting point. The native
keyboard offers standard text/numeric modes but not arbitrary custom layouts;
an optional button-matrix/custom component must implement extra locale layouts
rather than assuming unsupported YAML. See [source notes](SOURCES.md).
Provide es/en keyboard layouts and font coverage; UI locale and entered text
are distinct. Password validation must not strip spaces or normalize case.

Password fields mask by default, expose an explicit reveal action and clear
their transient buffer when the session ends. Reveal is not persisted. Never
log passwords, include them in telemetry, or reuse them as a displayed field value.
Existing stored passwords are represented as 'configured', not read back.
A cancelled edit leaves the saved credential unchanged.

Rotary editor flow: choose a character group (letters, digits, symbols), choose
a character, add it, then select Next/Backspace/Move cursor/Save/Cancel.
A large peer keyboard is the preferred convenience path for long credentials,
but basic local correction and cancellation must remain possible.

## Form behavior and acceptance cases

Labels remain visible separately from placeholder text. Error messages name the
field and corrective action. Validate cheap constraints while editing; validate
network/domain constraints on Apply. Scroll the first invalid field into view.
Async validation cannot overwrite newer edits: use a draft revision.

Apply creates one operation. Repeated presses while pending do not submit twice.
Cancellation distinguishes 'discard local draft' from 'remote operation already
committed'. Show actual final state when cancellation arrives too late.

Minimum fixtures: accented text, a maximum-length input, leading/trailing spaces,
empty password on an open network, unavailable network, long translated labels,
one missing glyph, disconnect during Save, error after view closes, unsupported
keyboard capabilities, and touch-to-encoder handover in the same form.

## Module extension recipe

1. Check whether an existing application/control expresses the intended action.
2. Register one module ID and configuration schema with bounded defaults.
3. Declare inputs/actions and provide a deterministic fake adapter.
4. Implement reusable behavior and profile-specific presentation.
5. Register locale keys/glyphs and all six input semantics.
6. Add one runnable composition, state fixtures and targeted conformance tests.
7. Document measured resource use, supported targets and known limitations.

Agent-readable metadata should enumerate fields, capabilities, examples and
status, generated from the validation source. An agent must be able to discover
that a keyboard is unsupported on a profile before it generates firmware.

Peer roles do not depend on screen size: a tiny encoder controller may operate
a larger panel, and a larger panel may provide a keyboard for a smaller target.
Negotiate capabilities and explicit session roles; see [peer roles](CONNECTIVITY.md).
