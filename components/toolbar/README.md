# Navigation toolbar

Uses the shared bar/surface.yaml geometry. Default placement is TOP_MID;
nabla_toolbar_align can change the anchor for another composition.
The example uses a 48-pixel header and an equally tall temporary footer.
The toolbar follows light/dark mode and has no divider or progress line.

The brand triangle spins on startup and rotates left on interior focus.
Activating it returns to the parent. The current title is bold, noninteractive
and excluded from focus; ancestor breadcrumbs remain selectable.
The right area uses Font Awesome: red X inside views, gray Wi-Fi/Bluetooth
placeholders at the root. These placeholders are not live connectivity.
Ordinary controls change only their border on focus.

Progress now belongs to components/progress/widget.yaml. The script API in
progress.yaml remains available for compatibility; negative values hide the
footer, 0..100 display it. See ../progress/README.md.

The logo container has a -3 px optical vertical offset relative to its centered
position. Its hit area stays fixed; rotation keeps the same local pivot.
