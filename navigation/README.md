# Shared navigation

The current prototype declares nodes in tree.h. Each node has a title, parent
index and optional leaf description. Node 0 is Inicio. Settings has Wi-Fi,
Idioma and Acerca de. Sensores demonstrates two folder levels.

logic.yaml provides nav_open(node), nav_back, nav_home, nav_move(delta),
nav_activate and nav_render. The touchscreen opens a node directly.
A future encoder can call move and activate rather than define another tree.
The list highlights the selected row. The home tiles currently select directly;
the desktop and list both show sequential keyboard focus.

The breadcrumb displays the complete route. Inicio goes to the root and Volver
goes to the parent. Intermediate breadcrumb segments are not individually clickable.
The list supports up to eight children per node and scrolls vertically.

The root icon grid is currently composed in launcher.yaml with node indices.
Keep titles and root entries consistent with tree.h; a unified declarative
catalog is a future improvement. This is a prototype contract.

Wi-Fi and language pages report pending implementation. They do not change
network settings or language. About is informational. Sensor leaves have no
connected entity and must not pretend to show real readings.

## Keyboard and touch

Focus the simulator window. Up/Down selects the previous/next item (wrapping
at the ends), Enter opens it, Escape goes back, Home returns to the desktop.
The selected border is white; other actionable borders are cyan. Touch selects
and opens through the same action, after 120 ms of visual feedback.
