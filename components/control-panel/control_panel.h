// LVGL control-panel grid and input helpers; renders consumer-defined sections and cells consistently.

#pragma once
#include "lvgl.h"
#include <string>
#include <vector>
#include <functional>

namespace nabla_control_panel {

enum class CellKind { Toggle, Momentary, Sensor };

struct Cell {
  const char* id;
  const char* label;
  CellKind kind;
  bool state = false;
  int value = 0;
  const char* unit = "";
  bool available = true;
};

struct Section {
  const char* id;
  const char* label;
  std::vector<Cell> cells;
};

struct Words {
  const char* on;
  const char* off;
  const char* unavailable;
  const char* press;
};

struct PanelState {
  std::vector<Section> sections;
  int current_section = 0;
  int focus_row = 0;
  int focus_col = 0;
  bool active = false;
  bool dense = false;
  Words words{};
  std::function<void(const char*, const char*, bool)> on_toggle;
  std::function<void(const char*, const char*)> on_momentary;
};

inline PanelState state;

inline int columns(int screen_w, bool dense) {
  if (dense) return screen_w >= 320 ? 5 : (screen_w >= 240 ? 4 : 3);
  return screen_w > 320 ? 4 : (screen_w > 200 ? 3 : 2);
}
inline int columns(int screen_w) { return columns(screen_w, state.dense); }
inline int rows_visible(int screen_h, int bar_height) { return (screen_h - bar_height * 2 - 40) / 36; }

inline void render_cell(lv_obj_t* btn, lv_obj_t* label, const Cell& cell, const Words& words,
                        bool dark, bool focused, int width, int height, bool dense = false) {
  lv_obj_set_size(btn, width, height);
  lv_obj_set_style_pad_all(btn, 0, 0);
  const bool lit = cell.available && cell.state;
  uint32_t bg_color = 0;
  uint32_t text_color = dark ? 0xFFFFFF : 0x000000;
  uint32_t border_color = focused ? (dark ? 0xFFFFFF : 0x000000) : (dark ? 0x404040 : 0xBFBFBF);

  if (cell.kind == CellKind::Sensor) {
    bg_color = dark ? 0x1A1A2E : 0xE8E8F0;
  } else if (!cell.available) {
    bg_color = dark ? 0x2A2A2A : 0xD0D0D0;
    text_color = 0x808080;
  } else if (lit) {
    bg_color = dark ? 0x2D4A1C : 0xD4EDBC;
  } else {
    bg_color = dark ? 0x1E1E1E : 0xF0F0F0;
  }

  lv_obj_set_style_bg_color(btn, lv_color_hex(bg_color), 0);
  lv_obj_set_style_border_color(btn, lv_color_hex(border_color), 0);
  lv_obj_set_style_border_width(btn, focused ? 2 : 1, 0);
  lv_obj_set_style_radius(btn, dense ? 2 : 4, 0);

  std::string text = cell.label;
  if (dense) {
    text += "\n";
    if (cell.kind == CellKind::Sensor) {
      text += cell.available ? (std::to_string(cell.value) + cell.unit) : "-";
    } else if (cell.kind == CellKind::Momentary) {
      text += cell.available ? ">" : "-";
    } else {
      text += !cell.available ? "-" : (cell.state ? words.on : words.off);
    }
  } else {
    text += "\n";
    if (cell.kind == CellKind::Sensor) {
      text += cell.available ? (std::to_string(cell.value) + cell.unit) : words.unavailable;
    } else if (cell.kind == CellKind::Momentary) {
      text += cell.available ? words.press : words.unavailable;
    } else {
      text += !cell.available ? words.unavailable : (cell.state ? words.on : words.off);
    }
  }

  lv_label_set_text(label, text.c_str());
  lv_obj_set_style_text_color(label, lv_color_hex(text_color), 0);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
  lv_obj_set_width(label, width - 4);
  lv_obj_set_style_pad_all(label, 0, 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

inline void render_tabs(lv_obj_t* container, const std::vector<Section>& sections,
                        int current, bool dark, int screen_w, int bar_h, bool dense = false,
                        const lv_font_t* font = nullptr) {
  static std::vector<lv_obj_t*> tab_btns;
  for (auto* btn : tab_btns) { if (btn) lv_obj_delete(btn); }
  tab_btns.clear();

  if (sections.size() <= 1) return;

  int tab_height = dense ? 18 : 24;
  int tab_gap = dense ? 2 : 4;
  int tab_width = (screen_w - tab_gap * ((int)sections.size() + 1)) / (int)sections.size();
  int x = tab_gap;
  int y = bar_h + (dense ? 1 : 2);
  for (size_t i = 0; i < sections.size(); i++) {
    lv_obj_t* btn = lv_button_create(container);
    lv_obj_set_size(btn, tab_width, tab_height);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_style_radius(btn, 2, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_pad_all(btn, 0, 0);

    bool selected = (int)i == current;
    lv_obj_set_style_bg_color(btn, lv_color_hex(selected ? (dark ? 0x404040 : 0xD0D0D0) : (dark ? 0x1E1E1E : 0xF5F5F5)), 0);
    lv_obj_set_style_border_width(btn, selected ? 2 : 1, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(selected ? (dark ? 0xFFFFFF : 0x000000) : (dark ? 0x404040 : 0xBFBFBF)), 0);

    lv_obj_t* lbl = lv_label_create(btn);
    lv_label_set_text(lbl, sections[i].label);
    lv_obj_set_style_text_color(lbl, lv_color_hex(dark ? 0xFFFFFF : 0x000000), 0);
    if (dense && font) lv_obj_set_style_text_font(lbl, font, 0);
    lv_obj_center(lbl);

    tab_btns.push_back(btn);
    x += tab_width + tab_gap;
  }
}

inline void render_sensor_row(lv_obj_t* container, const std::vector<Cell>& sensors,
                              const Words& words, bool dark, int y, int screen_w, bool dense = false,
                              const lv_font_t* font = nullptr) {
  if (sensors.empty()) return;
  int gap = dense ? 2 : 4;
  int cell_w = (screen_w - gap * ((int)sensors.size() + 1)) / std::max(1, (int)sensors.size());
  int cell_h = dense ? 16 : 24;

  static std::vector<lv_obj_t*> sensor_objs;
  for (auto* obj : sensor_objs) { if (obj) lv_obj_delete(obj); }
  sensor_objs.clear();

  int x = gap;
  for (const auto& cell : sensors) {
    lv_obj_t* box = lv_obj_create(container);
    lv_obj_set_size(box, cell_w, cell_h);
    lv_obj_set_pos(box, x, y);
    lv_obj_set_style_bg_color(box, lv_color_hex(dark ? 0x1A1A2E : 0xE8E8F0), 0);
    lv_obj_set_style_border_width(box, 0, 0);
    lv_obj_set_style_radius(box, 2, 0);
    lv_obj_set_style_pad_all(box, 0, 0);
    lv_obj_remove_flag(box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* lbl = lv_label_create(box);
    std::string text = cell.label;
    text += ":";
    text += cell.available ? (std::to_string(cell.value) + cell.unit) : "-";
    lv_label_set_text(lbl, text.c_str());
    lv_obj_set_style_text_color(lbl, lv_color_hex(dark ? 0xFFFFFF : 0x000000), 0);
    if (dense && font) lv_obj_set_style_text_font(lbl, font, 0);
    lv_obj_center(lbl);

    sensor_objs.push_back(box);
    x += cell_w + gap;
  }
}

inline int cell_count() {
  if (state.sections.empty()) return 0;
  int count = 0;
  for (const auto& cell : state.sections[state.current_section].cells) {
    if (cell.kind != CellKind::Sensor) count++;
  }
  return count;
}

inline Cell* focused_cell(int cols) {
  if (state.sections.empty()) return nullptr;
  auto& section = state.sections[state.current_section];
  int idx = 0;
  for (auto& cell : section.cells) {
    if (cell.kind != CellKind::Sensor) {
      if (idx == state.focus_row * cols + state.focus_col) return &cell;
      idx++;
    }
  }
  return nullptr;
}

inline void move(int dr, int dc, int cols) {
  int total = cell_count();
  if (total == 0) return;
  int rows = (total + cols - 1) / cols;
  int idx = state.focus_row * cols + state.focus_col;
  
  if (dr != 0) {
    int new_row = (state.focus_row + dr + rows) % rows;
    int new_idx = new_row * cols + state.focus_col;
    if (new_idx >= total) new_idx = total - 1;
    state.focus_row = new_idx / cols;
    state.focus_col = new_idx % cols;
  } else if (dc != 0) {
    idx = (idx + dc + total) % total;
    state.focus_row = idx / cols;
    state.focus_col = idx % cols;
  }
}

inline void activate(int cols) {
  Cell* cell = focused_cell(cols);
  if (!cell || !cell->available) return;
  
  const char* section_id = state.sections[state.current_section].id;
  if (cell->kind == CellKind::Toggle) {
    cell->state = !cell->state;
    if (state.on_toggle) state.on_toggle(section_id, cell->id, cell->state);
  } else if (cell->kind == CellKind::Momentary) {
    if (state.on_momentary) state.on_momentary(section_id, cell->id);
  }
}

inline void switch_section(int delta) {
  if (state.sections.size() <= 1) return;
  state.current_section = (state.current_section + delta + (int)state.sections.size()) % (int)state.sections.size();
  state.focus_row = 0;
  state.focus_col = 0;
}

}
