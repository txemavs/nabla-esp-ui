# M1 host verification

Verified on 2026-09-16 with pinned ESPHome 2026.8.2 and WSLg/SDL.
This records simulator evidence; it does not certify any OLED, e-paper or ESP board.

## Delivered

Named regular, tiny and readable profiles; actual 480x320, 320x480 and 128x64
viewports; one YAML navigation tree; shared focus/scroll policies; optional
compact application footer; unchanged large-screen form editing.
The 128x64 renderer has both three-row and single-large-item presentation.
Root triangle toggles density; all eight root applications remain accessible.

## Checks completed

- Seven unittest cases passed, including C++ profile/model assertions.
  The trace fixture visits all eight root routes via sequential and touch-style
  activation in each compact density and compares the destinations.
- Nested entry, parent return, first-content focus on re-entry, root density
  change, theme actions and last-row scroll visibility are covered.
- Geometry checks cover regular landscape/portrait and both compact densities,
  with/without footer. Required content and footer regions do not overlap.
- Compiled regular, portrait, compact Spanish/English, readable and an
  application-footer-enabled compact variant.
- Inspected the actual 320x480 grid and native 128x64 three-row/large-item views.
- Exercised compact theme selection with Up/Down/Enter and scrolled to option
  eight; the selected last row stayed visible with footer 8/8.
- Pixel checks on dark/light and readable compact captures found only
  (0,0,0) and (255,255,255). No grayscale font smoothing.
- Font sizes: title/footer 8 px, tiny body 10 px, readable body 16 px.
  Root rows have 13/40 px heights respectively; the two bars each have 12 px.
  This measures raster fit, not a claim about readable physical text size.

## Remaining in later phases

Compact text/password editing and paged detail readers; actual input GPIOs,
display drivers, physical legibility testing, resource/latency measurements
on ESP hardware; network adapters and dynamic remote menu transport.
The Wi-Fi leaf explicitly marks its compact editor as M2 work.
No general production profile API or hardware-support claim is implied.
