# Brand identity

The personal mark represents an equilateral, acute silver triangle reflecting
the sky. Its blue is a reflection, not a restriction to flat blue material.
The owner created the mark at age 14; preserve continuity with the original
artwork in assets/nabla.jpg.

Geometry: three equal outer sides, a substantial triangular ring and sharp
corners. The current editable SVG uses three bands with three faces each:
nine geometric faces. Gradients suggest polished silver and reflected sky.

The owner's preferences include 3, 6, 9 and the golden ratio. Use them
thoughtfully for rhythm, subdivisions and proportions, without compromising
legibility, accessibility or exact equilateral geometry. Current reflection
stops use approximately 0.382 and 0.618.

Motion has two distinct forms:
- Intro: a 1000 ms spindle-like depth-turn approximation. Horizontal
  foreshortening changes; vertical coordinates and bottom tip remain
  fixed. This is not a full perspective 3D mesh.
- Toolbar: a 180 ms in-plane 180-degree rotation around the triangle centroid.
  It turns in the same direction to point upward toward the parent.
  The SVG centroid and rotation pivot both lie at the canvas center.
The historical GIF may guide a later, more faithful depth animation.
Avoid perpetual motion; preserve the one-second intro duration.

Do not overwrite the original JPG. Keep geometric sources editable and
animation logic reusable. Support reduced/disabled motion as a future option.

UI palette: the triangle uses a configurable solid accent (cyan by default).
Its focused state is white; an error may change the underlying status color.
Ordinary control focus changes borders. App cards may have their own colors.
The header has no divider. Shared header/footer follow dark/light mode; idle
brand text is gray, while the startup progress footer is gray/cyan.
Preserve the silver/sky artwork as historical reference, not a mandatory raster
texture on every display. Monochrome/readable profiles must preserve recognizability.

Navigation rotation uses a fixed pivot with no per-angle translation. The toolbar
logo is placed 3 px lower than the previous -3 px optical offset (now y = 0).
