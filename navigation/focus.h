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
}
