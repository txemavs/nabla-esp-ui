#pragma once
#include "model.h"
#include "geometry.h"
#include "esphome/components/display/display.h"
#include "esphome/components/font/font.h"
namespace nabla {
class CompactShell {
 public:
  CompactMenu menu;
  bool app_footer = false;
  int last_node = -1, last_focus = -1;
  bool last_readable = false;
  uint32_t focus_since = 0;
  void render(esphome::display::Display &d, esphome::font::Font *small,
              esphome::font::Font *body, esphome::font::Font *large,
              const char *back_text, const char *pending) {
    using namespace esphome;
    if (last_node != menu.current || last_focus != menu.focus || last_readable != menu.readable) {
      last_node = menu.current; last_focus = menu.focus; last_readable = menu.readable;
      focus_since = millis();
    }
    auto g = Geometry::compact(menu.readable, menu.current == 0 || app_footer);
    Color fg = menu.dark ? Color::WHITE : Color::BLACK;
    Color bg = menu.dark ? Color::BLACK : Color::WHITE;
    d.fill(bg);
    bool header_focus = menu.focus == children(menu.current);
    // Equilateral triangle, fixed center. Up on parent focus, down otherwise.
    if (header_focus && menu.current)
      d.triangle(2, 10, 12, 10, 7, 1, fg);
    else d.triangle(2, 1, 12, 1, 7, 10, fg);
    if (header_focus && !menu.current) d.rectangle(0, 0, 15, 12, fg);
    d.start_clipping(16, 0, 127, 11);
    d.print(16, 0, small, fg, nodes[menu.current].title);
    d.end_clipping();
    const int n = children(menu.current);
    if (n) {
      for (int r = 0; r < g.rows; ++r) {
        int index = menu.top + r;
        if (index >= n) break;
        int y = g.header + r * g.row_height;
        bool selected = menu.focus == index;
        if (selected) d.rectangle(0, y, 128, g.row_height, fg);
        auto *font = menu.readable ? large : body;
        const char *title = nodes[child(menu.current, index)].title;
        int bx, by, bw, bh;
        d.get_text_bounds(0, 0, title, font, display::TextAlign::TOP_LEFT, &bx,&by,&bw,&bh);
        // Bounded marquee exposes complete long labels without changing focus.
        int offset = 0;
        if (selected && bw > 120) {
          int range = bw - 120;
          int phase = ((millis()-focus_since)/100) % (2*range + 20);
          offset = std::min(range, std::max(0, phase-10));
          if (phase > range+10) offset = std::max(0, 2*range+10-phase);
        }
        d.start_clipping(3, y+1, 124, y+g.row_height-2);
        d.print(4-offset, y+(g.row_height-bh)/2, font, fg, title);
        d.end_clipping();
      }
    } else {
      d.start_clipping(2, 14, 126, 64-g.footer-16);
      const char *detail = nodes[menu.current].action == 3 ? pending : nodes[menu.current].detail;
      d.print(2, 15, body, fg, detail);
      d.end_clipping();
      // Header is the semantic Back target; provide an obvious touch-sized row.
      d.rectangle(0, 64-g.footer-14, 128, 14, fg);
      d.print(4, 64-g.footer-14, body, fg, back_text);
    }
    if (g.footer) {
      d.print(2, 54, small, fg, "NABLA");
      if (n) d.printf(126, 54, small, fg, display::TextAlign::TOP_RIGHT,
                     "%d/%d", menu.focus < n ? menu.focus+1 : std::min(menu.top+1,n), n);
    }
  }
  void touch(int x, int y) {
    if (y < 12) { if (x < 16) menu.touch_option(children(menu.current)); return; }
    if (!children(menu.current)) { if (y >= 64-(app_footer ? 12 : 0)-14 && y < 64-(app_footer ? 12 : 0)) menu.back(); return; }
    auto g = Geometry::compact(menu.readable, menu.current == 0 || app_footer);
    if (y >= 64-g.footer) return;
    int row = (y-12)/g.row_height;
    if (row < g.rows) menu.touch_option(menu.top + row);
  }
};
inline CompactShell compact_shell;
}
