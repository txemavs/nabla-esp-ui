# Toolbar

Shared black shell with cyan bottom divider, drawn Home logo, breadcrumb
segments and X. The desktop has a title and no X. Splash hides the toolbar.

The Home logo points left when focused in an application, without inverting
its background. Home still opens the desktop; X returns to the parent.
Other focused sections invert. Touch and sequential input share activation.

Depends on navigation actions/state and theme fonts/colors.
Current widget IDs are shared singleton IDs; include this toolbar once in
LVGL top_layer. It is not yet a multi-instance toolbar.
See navigation/README.md for focus order and breadcrumb truncation.
