// Compact menu state and actions; shares navigation semantics without requiring LVGL.

#pragma once
#include "esphome/components/nabla_navigation/navigation.h"
#include "focus.h"
#include <vector>
#include <functional>
namespace nabla {
struct CompactMenu {
  int current = 0, focus = 0, top = 0;
  int root_content_focus = 0; // Preserve the item while the view-toggle logo has focus.
  bool dark = true, readable = false, borders = true;
  int font_family = 0;
  int list_rows = 3;
  std::vector<int> saved_focus = std::vector<int>(count, 0);
  std::vector<int> saved_top = std::vector<int>(count, 0);
  // Callback for view toggle (list <-> icon) at root.
  std::function<void()> on_view_toggle;
  int total() const { return children(current) + 1; } // reachable logo/Back
  int rows() const { return readable ? 1 : list_rows; }
  void anchor() {
    // Header focus must not force the list to scroll.
    if (focus < children(current)) {
      if(current==0)root_content_focus=focus;
      top = scroll_anchor(focus, top, rows(), children(current));
    }
  }
  void move(int delta) {
    anchor();
    focus = wrap_focus(focus, delta, total());
    // Passing below the last root item previews the next Down destination.
    if(current==0 && delta>0 && focus==children(0)){
      top=0;
      root_content_focus=0;
    }
    anchor();
  }
  void open(int node) {
    if (!valid(node)) return;
    if (nodes[node].action == 5 || nodes[node].action == 6 || nodes[node].action == 7) {
      if (nodes[node].action == 5) dark = !dark; else if (nodes[node].action == 7) borders = !borders; else font_family = (font_family + 1) % 2;
      node = nodes[node].parent;
    }
    if (nodes[node].action == 1 || nodes[node].action == 2) {
      dark = nodes[node].action == 1;
      node = nodes[node].parent;
    }
    if (nodes[node].action == 8) { execute_command(node); node = nodes[node].parent; }
    if (node == current) return;
    saved_focus[current] = focus; saved_top[current] = top;
    bool entering = nodes[node].parent == current;
    current = node;
    focus = content_focus(saved_focus[node], children(node), entering);
    top = entering ? 0 : saved_top[node];
    anchor();
  }
  void back() { if (current) open(nodes[current].parent); }
  void home() { open(0); }
  void activate() {
    if (focus < children(current)) open(child(current, focus));
    else if (current) back();
    else {
      readable = !readable;
      focus = content_focus(root_content_focus, children(0), false);
      anchor();
      if (on_view_toggle) on_view_toggle();
    }
  }
  void touch_option(int index) {
    if (index < 0 || index >= total()) return;
    anchor(); focus = index; activate();
  }
};
}
