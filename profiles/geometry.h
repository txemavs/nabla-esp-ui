#pragma once
#include <algorithm>
namespace nabla {
struct Geometry {
  int width, height, header, footer, rows, row_height, margin, gap;
  static Geometry regular(int w, int h, bool footer_visible, int bar = 36, bool dense = false) {
    return {w, h, bar, footer_visible ? bar : 0, 0, dense ? 32 : 48, 4, dense ? 2 : 6};
  }
  static Geometry compact(bool readable, bool footer_visible) {
    int header = 12, footer = footer_visible ? 12 : 0;
    int rows = readable ? 1 : 3;
    return {128, 64, header, footer, rows, (64-header-footer)/rows, 1, 0};
  }
  int content_height() const { return height - header - footer; }
};
}
