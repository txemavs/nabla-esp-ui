#pragma once
#include "esphome/core/defines.h"
#ifdef USE_LVGL
#include "lvgl.h"
namespace nabla_style {
inline bool borders = true;
inline lv_color_t idle(bool dark) { return lv_color_hex(dark ? 0x404040 : 0xBFBFBF); }
inline void text(lv_obj_t *o, lv_color_t color) {
  lv_obj_set_style_text_color(o,color,0);
  for (unsigned i=0;i<lv_obj_get_child_count(o);++i) text(lv_obj_get_child(o,i),color);
}
inline void control(lv_obj_t *o, bool selected, bool dark, bool mono=false) {
  auto fg=lv_color_hex(dark?0xFFFFFF:0), bg=lv_color_hex(dark?0:0xFFFFFF);
  bool inverse=!borders && selected;
  lv_obj_set_style_border_width(o,borders?(selected?2:mono?0:1):0,0);
  lv_obj_set_style_border_width(o,borders?2:0,LV_STATE_PRESSED);
  lv_obj_set_style_border_color(o,selected?fg:idle(dark),0);
  lv_obj_set_style_border_color(o,fg,LV_STATE_PRESSED);
  lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);
  lv_obj_set_style_bg_opa(o,LV_OPA_COVER,LV_STATE_PRESSED);
  lv_obj_set_style_bg_color(o,inverse?fg:bg,0);
  lv_obj_set_style_bg_color(o,borders?bg:fg,LV_STATE_PRESSED);
  text(o,inverse?bg:fg);
  lv_obj_set_style_text_color(o,borders?fg:bg,LV_STATE_PRESSED);
}
}

#endif
