# Logo

A line-drawn cyan outline triangle with two orientations, down and left.
No image file or font glyph is needed at runtime.

Include widget.yaml with widget_id (unique), size (pixels), stroke (pixels).
Choose a size larger than four times the stroke width.
Root is centered in its parent. All geometry is non-clickable so the containing
button receives touch events. The parent owns the action and input behavior.

The generated IDs widget_id_down and widget_id_left select orientation by
toggling LV_OBJ_FLAG_HIDDEN. The toolbar points left only when focused on an
interior view; its action remains Home. This is an immediate state change,
not an animated rotation. Desktop uses a subtle focus outline.
The splash uses the same component at a larger size.

The original artwork remains in assets/nabla.jpg as a brand reference.
