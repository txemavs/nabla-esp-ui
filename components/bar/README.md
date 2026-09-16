# Bar surface

surface.yaml defines common geometry for header and footer: full width, shared
nabla_bar_height (48 by default), no border, padding or radius.
Merge it into an LVGL obj and supply bar_id, anchor (TOP_MID or BOTTOM_MID),
and background. Add the content as widgets. Instances must have unique IDs.

The minimum composition is a header, body text and optional footer.
A smaller profile may choose text only or header/footer lines. These are
composition goals; the current launcher still targets 480 x 320.
Changing the bar anchor does not automatically reflow an arbitrary page.
The example reserves footer space during progress and restores it afterward.

The desktop footer is always present. After startup reaches 100% (held for
100 ms), progress is replaced by Soluciones Lógicas Nabla on the left and
HH:MM on the right. The shared footer_set_clock(text) action accepts clock
updates from hardware adapters; the simulator uses host time, Europe/Madrid.
Application views hide the idle desktop footer and may later supply their own.
Negative progress restores this route-specific state rather than hiding all footers.

Compact layout: shared bars are 36 px tall; launcher outer padding is 5 px
horizontally and 6 px vertically. Internal tile gaps stay 10 px. Idle footer
follows the header theme; gray/cyan is reserved for the startup progress state.
