#pragma once
#include "lvgl.h"
namespace nabla_camera {
inline lv_obj_t *thumbnail_image=nullptr;
inline void thumbnail(lv_obj_t *button,lv_obj_t *label,const lv_image_dsc_t *image,
                      bool ready,bool dark,bool focused,int width,int height) {
  lv_obj_set_size(button,width,height);
  lv_obj_set_style_pad_all(button,4,0);
  lv_label_set_text(label,"");
  for(unsigned j=1;j<lv_obj_get_child_count(button);++j)
    lv_obj_add_flag(lv_obj_get_child(button,j),LV_OBJ_FLAG_HIDDEN);
  // Preserve the light renderer's fixed label/icon/bar child slots.
  if(lv_obj_get_child_count(button)<2)lv_label_create(button);
  if(lv_obj_get_child_count(button)<3) {
    auto *bar=lv_bar_create(button);
    lv_obj_add_flag(bar,LV_OBJ_FLAG_HIDDEN);
  }
  if(!thumbnail_image) {
    thumbnail_image=lv_image_create(button);
    lv_obj_remove_flag(thumbnail_image,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(thumbnail_image,LV_OBJ_FLAG_IGNORE_LAYOUT);
  } else if(lv_obj_get_parent(thumbnail_image)!=button) {
    lv_obj_set_parent(thumbnail_image,button);
  }
  lv_obj_set_style_bg_color(button,lv_color_hex(dark?0x181818:0xF1F1F1),0);
  lv_obj_set_style_border_width(button,focused?2:1,0);
  lv_obj_set_style_border_color(button,lv_color_hex(focused?(dark?0xFFFFFF:0):(dark?0x383838:0xDDDDDD)),0);
  if(ready && image) {
    lv_image_set_src(thumbnail_image,image);
    lv_image_set_scale(thumbnail_image,128); // 64 px source in a 32 px icon space.
    lv_obj_center(thumbnail_image);
    lv_obj_remove_flag(thumbnail_image,LV_OBJ_FLAG_HIDDEN);
  }
}
inline void hide_thumbnail() {
  if(thumbnail_image)lv_obj_add_flag(thumbnail_image,LV_OBJ_FLAG_HIDDEN);
}
}
