#pragma once
#include <algorithm>
namespace nabla {
inline int wrap_focus(int selected, int delta, int total) {
  return total > 0 ? (selected + delta % total + total) % total : 0;
}
inline int content_focus(int saved, int children, bool entering) {
  return entering || saved < 0 || saved >= children ? 0 : saved;
}
inline int scroll_anchor(int selected, int top, int rows, int total) {
  rows = std::max(1, rows);
  top = std::max(0, std::min(top, std::max(0, total - rows)));
  if (selected < top) top = selected;
  if (selected >= top + rows) top = selected - rows + 1;
  return std::max(0, top);
}
// Middle-scroll: keep selection in the middle row when possible.
// At list start, selection is on top rows; at end, on bottom rows.
// Provides a visual cue when reaching list boundaries.
inline int scroll_anchor_middle(int selected, int rows, int total) {
  rows = std::max(1, rows);
  if (total <= rows) return 0;  // All items fit, no scrolling needed.
  int mid = rows / 2;  // Middle row index (0-based).
  // Near start: selection appears on its natural row (0, 1, ..., mid).
  if (selected <= mid) return 0;
  // Near end: last items fill bottom rows.
  if (selected >= total - (rows - mid)) return total - rows;
  // Middle of list: keep selection on middle row.
  return selected - mid;
}
}
