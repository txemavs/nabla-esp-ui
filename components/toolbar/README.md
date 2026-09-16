# Toolbar

Shared black shell with medium-gray bottom divider, drawn Home logo, breadcrumb
segments and X. The desktop has a title and no X. Splash hides the toolbar.

The Home logo points left when focused in an application, without inverting
its background. Home still opens the desktop; X returns to the parent.
Other focused controls change only their border. Touch and sequential input share activation.

Depends on navigation actions/state and theme fonts/colors.
Current widget IDs are shared singleton IDs; include this toolbar once in
LVGL top_layer. It is not yet a multi-instance toolbar.
See navigation/README.md for focus order and breadcrumb truncation.

Current presentation contract: ordinary control focus changes only borders, never icon, text or
background colors. Icons and names default to white. The current toolbar title
is bold white and excluded from focus; ancestors remain regular clickable text.
The single underline starts just after the logo's lower tip. Startup spins the
same toolbar logo in place for one second; there is no separate splash page.
Set nabla_monochrome: "true" to hide unselected control borders. This is a
presentation option, not validation of physical OLED hardware.

The brand triangle is the focus exception: inside a view it rotates 90 degrees
left over 180 ms, without a focus box. Activating it returns to the parent,
just like X/ESC. Removing focus restores its downward orientation. At the root
it remains the Home mark. The underline meets its resting lower tip.
