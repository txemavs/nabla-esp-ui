# Silver triangle logo

silver.svg is the editable geometric source: equilateral outer and inner
triangles, three bands and nine faces. Reflections evoke silver and blue sky.
The original JPG remains a separate reference.

assets.yaml compiles transparent RGB565 assets at 180 and 36 pixels. The source
is vector geometry; the device renders precomputed pixels rather than an SVG
engine. Extra sizes should be declared in this package.

Include widget.yaml with unique widget_id, size and image_id.
widget_id_image is the single image used for both orientations.
widget_id_image identifies the unrotated image for animations.

nabla_logo_spin(target: lv_obj_t*) performs one spindle-like cycle in 1000 ms,
using horizontal foreshortening with the bottom tip anchored.
nabla_logo::turn(image, left) performs a centered planar 90-degree transition
in 180 ms; repeated renders do not restart it.
The object must remain alive during animation. The splash runs it once and
opens the desktop at 1600 ms. Actual ESP rendering performance remains untested.
See docs/BRAND.md for the identity and motion rules.

## Color and status

The widget recolors the entire silhouette with nabla_accent (default cyan).
The SVG reflections remain an editable reference, not the default UI shading.
Motion preserves the current color. Override nabla_accent with 0xFFFFFF for a
monochrome palette; physical monochrome display support still needs validation.

Change one instance at runtime, for example on an actual error event:

```yaml
- script.execute:
    id: nabla_logo_set_color
    target: !lambda return id(toolbar_logo_image);
    color: ${nabla_error}
```

Use ${nabla_accent} to restore its normal color. Each instance is independent.
The library exposes this action; automatic error detection is not implemented.
