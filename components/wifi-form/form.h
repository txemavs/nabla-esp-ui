#pragma once
#include "lvgl.h"
#include <cstring>
namespace nabla_wifi_form {
// Bounded, local editor state. Credentials live only in the textarea.
struct Editor {
  lv_obj_t *page{}, *ssid{}, *password{}, *keyboard{}, *status{}, *apply{}, *cancel{};
  int focus = 0;
  bool editing_password = false;
  bool dark_theme = true;
  void clear() {
    lv_textarea_set_text(ssid, "");
    lv_textarea_set_text(password, "");
    focus = 0;
    editing_password = false;
    lv_keyboard_set_textarea(keyboard, ssid);
  }
  void edit(bool secret) {
    editing_password = secret;
    focus = secret ? 1 : 0;
    lv_keyboard_set_textarea(keyboard, secret ? password : ssid);
  }
  int keys() const {
    int n = 0;
    while (n < 128 && lv_buttonmatrix_get_button_text(keyboard, n)) ++n;
    return n;
  }
  void highlight() {
    auto mark = [&](lv_obj_t *o, bool selected) {
      lv_obj_set_style_border_width(o, selected ? 2 : 1, 0);
      lv_obj_set_style_border_color(o, lv_color_hex(selected ? (dark_theme ? 0xFFFFFF : 0x000000) : 0x808080), 0);
    };
    int n = keys();
    mark(ssid, focus == 0); mark(password, focus == 1);
    mark(apply, focus == n + 2); mark(cancel, focus == n + 3);
    if (focus >= 2 && focus < n + 2) {
      lv_obj_add_state(keyboard, LV_STATE_FOCUSED);
      lv_buttonmatrix_set_selected_button(keyboard, focus - 2);
    } else {
      lv_obj_remove_state(keyboard, LV_STATE_FOCUSED);
      lv_buttonmatrix_set_selected_button(keyboard, LV_BUTTONMATRIX_BUTTON_NONE);
    }
  }
  void move(int delta) {
    int total = keys() + 4;
    focus = (focus + delta % total + total) % total;
    highlight();
  }
  void activate() {
    int n = keys();
    if (focus < 2) {
      edit(focus == 1);
      focus = 2;
      highlight();
    } else if (focus < n + 2) {
      lv_obj_send_event(keyboard, LV_EVENT_VALUE_CHANGED, nullptr);
      // Case/symbol keys may change the button count.
      if (focus >= keys() + 2) focus = 2;
      highlight();
    } else {
      lv_obj_send_event(focus == n + 2 ? apply : cancel, LV_EVENT_CLICKED, nullptr);
    }
  }
  int validation() const {
    size_t s = strlen(lv_textarea_get_text(ssid));
    size_t p = strlen(lv_textarea_get_text(password));
    if (s == 0 || s > 32) return 1;
    if (p < 8 || p > 63) return 2;
    return 0;
  }
  void render(bool dark) {
    dark_theme = dark;
    int w = lv_display_get_horizontal_resolution(lv_display_get_default());
    int h = lv_display_get_vertical_resolution(lv_display_get_default());
    bool portrait = w < h;
    int field_w = portrait ? w - 12 : (w - 18) / 2;
    lv_obj_set_pos(ssid, 6, 42); lv_obj_set_size(ssid, field_w, 40);
    lv_obj_set_pos(password, portrait ? 6 : field_w + 12, portrait ? 86 : 42);
    lv_obj_set_size(password, field_w, 40);
    int status_y = portrait ? 132 : 88;
    lv_obj_set_pos(status, 6, status_y); lv_obj_set_width(status, w - 12);
    int keyboard_y = status_y + 24;
    lv_obj_set_pos(keyboard, 4, keyboard_y);
    lv_obj_set_size(keyboard, w - 8, h - keyboard_y - 42);
    lv_obj_set_pos(apply, 6, h - 37); lv_obj_set_size(apply, (w - 18) / 2, 32);
    lv_obj_set_pos(cancel, (w - 18) / 2 + 12, h - 37);
    lv_obj_set_size(cancel, (w - 18) / 2, 32);
    auto bg = lv_color_hex(dark ? 0 : 0xFFFFFF);
    lv_obj_set_style_bg_color(page, bg, 0);
    lv_obj_set_style_bg_color(keyboard, bg, 0);
    lv_obj_set_style_text_color(status, lv_color_hex(dark ? 0xAAAAAA : 0x505050), 0);
    auto fg = lv_color_hex(dark ? 0xFFFFFF : 0x000000);
    for (auto *control : {ssid, password, apply, cancel}) {
      lv_obj_set_style_bg_color(control, bg, 0);
      lv_obj_set_style_bg_color(control, bg, LV_STATE_PRESSED);
      lv_obj_set_style_text_color(control, fg, 0);
      lv_obj_set_style_border_color(control, fg, LV_STATE_PRESSED);
    }
    for (auto *field : {ssid, password}) {
      lv_obj_set_style_text_color(field, lv_color_hex(0x808080), LV_PART_TEXTAREA_PLACEHOLDER);
      lv_obj_set_style_bg_color(field, fg, LV_PART_CURSOR);
    }
    for (int state : {0, int(LV_STATE_CHECKED), int(LV_STATE_FOCUSED),
                      int(LV_STATE_PRESSED), int(LV_STATE_FOCUSED | LV_STATE_PRESSED),
                      int(LV_STATE_CHECKED | LV_STATE_FOCUSED), int(LV_STATE_CHECKED | LV_STATE_PRESSED)}) {
      lv_obj_set_style_bg_color(keyboard, bg, LV_PART_ITEMS | state);
      lv_obj_set_style_text_color(keyboard, fg, LV_PART_ITEMS | state);
      lv_obj_set_style_border_color(keyboard,
          state & (LV_STATE_FOCUSED | LV_STATE_PRESSED) ? fg : lv_color_hex(0x808080),
          LV_PART_ITEMS | state);
    }
    highlight();
  }
};
inline Editor editor;
}
