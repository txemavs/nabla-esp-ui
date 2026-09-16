# Shared input contract

Hardware adapters emit semantic actions; views do not know GPIOs or SDL keys.
Canonical actions: UP, DOWN, LEFT, RIGHT, ENTER, ESC.
Home is an optional additional action, distinct from the context-sensitive logo.

## Current behavior

UP/DOWN traverse menu order and wrap; list focus scrolls into view.
ENTER activates; ESC returns to the parent, and is a no-op at the root.
Keyboard Home opens the root. LEFT/RIGHT are reserved until a control defines them.
Touch/mouse selects and activates through the same 120 ms feedback path.

Root focus: entries, triangle, rotation button. Root triangle toggles tiles/list.
Interior focus: entries, triangle, visible ancestors, X. Triangle and X go back.
Current title is bold, noninteractive and excluded from focus.
Status icons and idle footer text have no action.

Triangle focus recolors the mark white without a frame, preserving its base/status
color outside focus. Interior focus also rotates it left. Ordinary control focus
changes borders, not application colors. Monochrome mode hides unselected borders.
Clearing focus restores the triangle's normal color and orientation.

## Adapters

Touch and SDL keyboard are implemented. A mouse is touch-equivalent.
Physical rotary and joystick adapters remain planned:
- Rotary provides UP/DOWN and ENTER.
- Optional dedicated back button provides ESC.
- Five-way joystick provides four directions and center ENTER.
- Keyboard provides corresponding keys.

Every device without physical ESC must have a sequentially reachable on-screen
Back/Cancel. No essential operation may require LEFT/RIGHT, a long press or a
gesture exclusively. Physical adapters own debounce and release/repeat behavior.

## Extension contract

Distinguish focus from activation: moving focus must never execute an action.
Define edit mode separately from browse mode; fields need reachable Save/Cancel.
Preserve focus by stable semantic key across refresh, rotation and navigation.
Modal overlays own focus temporarily and restore it on close.
Remote sessions must release held input on disconnect and offer local escape.

Forms, editing and remote input are design work, not implemented behavior.
See [component/input plan](../docs/platform/COMPONENTS-AND-INPUT.md) for the target.
