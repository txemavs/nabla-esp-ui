# Bundled fonts

- DejaVu Sans: copied from Ubuntu fonts-dejavu-core; see DejaVu-LICENSE.txt.
- Font Awesome Free Solid 6.7.2: https://github.com/FortAwesome/Font-Awesome/tree/6.7.2
  See FontAwesome-LICENSE.txt (font files: SIL OFL 1.1).

Text sizes and compiled glyphs are defined in theme/fonts.yaml.
Spanish accents, uppercase accents, ñ, ü, inverted punctuation, degrees and euro
are included alongside printable ASCII. This is not full Unicode coverage.

Icon names are registered in theme/icons.yaml. To add an icon, add its codepoint
to the Font Awesome glyph list in theme/fonts.yaml, then define its named
substitution in theme/icons.yaml. Render icons with nabla_icons_32; use
nabla_font_16/20/28 for text. Only Free Solid icons in the bundled version apply.

fa-brands-400.ttf: Font Awesome Free Brands 6.7.2, from the official
FortAwesome/Font-Awesome release; covered by FontAwesome-LICENSE.txt.
Used for the Bluetooth toolbar glyph.
