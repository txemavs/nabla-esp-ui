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
