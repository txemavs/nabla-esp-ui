# Silver triangle logo

silver.svg is the editable geometric source: equilateral outer and inner
triangles, three bands and nine faces. Reflections evoke silver and blue sky.
The original JPG remains a separate reference.

assets.yaml compiles transparent RGB565 assets at 180 and 36 pixels. The source
is vector geometry; the device renders precomputed pixels rather than an SVG
engine. Extra sizes should be declared in this package.

Include widget.yaml with unique widget_id, size and image_id.
Generated IDs widget_id_down and widget_id_left control orientation.
widget_id_image identifies the unrotated image for animations.

nabla_logo_spin(target: lv_obj_t*) performs one eased 360-degree turn in 1000 ms.
The object must remain alive during animation. The splash runs it once and
opens the desktop at 1600 ms. Actual ESP rendering performance remains untested.
See docs/BRAND.md for the identity and motion rules.
