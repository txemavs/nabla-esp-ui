#pragma once
#include <algorithm>
namespace nabla {
struct Geometry {
  int width, height, header, footer, rows, row_height, margin, gap;
  static Geometry regular(int w, int h, bool footer_visible, int bar = 36, bool dense = false) {
    return {w, h, bar, footer_visible ? bar : 0, 0, dense ? 32 : 48, 4, dense ? 2 : 6};
  }
  static Geometry compact(bool readable, bool footer_visible, int width=128, int height=64, int bar=12, int list_rows=3) {
    int header = bar, footer = footer_visible ? bar : 0;
    int rows = readable ? 1 : list_rows;
    return {width, height, header, footer, rows, (height-header-footer)/rows, 1, 0};
  }
  int content_height() const { return height - header - footer; }
};
}
