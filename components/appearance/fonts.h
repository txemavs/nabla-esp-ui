#pragma once
#include "lvgl.h"
#include "esphome/components/nabla_navigation/control_style.h"
namespace nabla_appearance {
struct FontPair { const lv_font_t *regular; const lv_font_t *ubuntu; };
// Children first: inspect inherited fonts before changing their parent.
inline void apply(lv_obj_t *obj, const FontPair *pairs, unsigned count, int family) {
  if (!obj) return;
  for (unsigned i=0; i<lv_obj_get_child_count(obj); ++i)
    apply(lv_obj_get_child(obj,i), pairs,count,family);
  for (auto part : {LV_PART_MAIN, LV_PART_ITEMS, LV_PART_SELECTED, LV_PART_CURSOR}) {
    auto *current=lv_obj_get_style_text_font(obj,part);
    for (unsigned i=0; i<count; ++i) {
      if (current==pairs[i].regular || current==pairs[i].ubuntu) {
        const auto *target = family==0?pairs[i].ubuntu:pairs[i].regular;
        if (current != target) lv_obj_set_style_text_font(obj,target,part);
        break;
      }
    }
  }
}
}
