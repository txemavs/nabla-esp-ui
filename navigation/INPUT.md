# Shared input contract

This contract applies to every nabla.net ESP UI device and presentation.
Hardware adapters emit semantic actions; views must not depend on GPIO pins,
SDL key codes, or a particular physical input device.

## Core actions

- Previous / Up: select the previous item in the view's defined order.
- Next / Down: select the next item; lists scroll to keep focus visible.
- Confirm: activate the selected item, or enter/finish editing a value.
- Back: close the current view and return to its parent/previous context.
- Home: return to the application desktop.
- Left / Right: separate directional actions for controls that explicitly
  support them. They do not duplicate sequential menu navigation by default.

In the current launcher, sequential order is row-major: traverse the first row
left to right, then the next row. Up/Down follows that sequence and wraps.
A menu list follows its displayed order.

## Device adapters

- Touchscreen: tapping an item selects and activates it.
- Mouse: clicking behaves like tapping.
- Rotary encoder: rotation emits Previous/Next; pressing emits Confirm.
- Five-way joystick: Up, Down, Left, Right and center press (Confirm).
- Optional Escape/back button: emits Back.
- Simulator keyboard: Up/Down, Enter, Escape and Home respectively.
  Left/Right are reserved until a view implements directional interaction.

Devices without a dedicated Back button must still expose an on-screen,
focusable route back. A physical Home button is optional.

## Visual and interaction rules

- Actionable controls use cyan; selected/pressed controls have a white border.
- Touch and keyboard confirmation share the same activation path.
- Current activation feedback lasts 120 ms before opening a view.
- Focus and activation are distinct: moving focus never opens an application.
- Mixed input must work without switching modes.
- Controls that edit values must distinguish navigation from editing visually.
- Future toolbar actions must participate in the focus order.
- Returning to a view should restore its prior focus and scroll position.

## Current implementation and pending work

SDL Up/Down calls nav_move(-1/+1), Enter calls nav_activate, Escape calls
nav_back, and Home calls nav_home. Touch uses nav_choose(index).
Hardware encoder and joystick adapters are not implemented yet.
Left/Right control-specific behavior, edit mode, focusable shared toolbar,
and restoration of focus/scroll on return remain to be implemented.
