#pragma once
#include "lvgl.h"
#include "esphome/components/nabla_navigation/control_style.h"
#include "esphome/components/nabla_navigation/forms.h"
namespace nabla_forms {
struct RegularForm {
  Controller model;
  lv_obj_t *page=nullptr,*title=nullptr,*body=nullptr;
  std::array<lv_obj_t *,10> buttons{};
  struct Context {RegularForm *self;int index;};
  std::array<Context,10> context{};
  void (*on_close)()=nullptr;
  bool dark=true,mono=false;
  void setup(lv_obj_t *root,const lv_font_t *font) {
    if(page)return;
    page=root;model.session.configure(fields,field_count);
    lv_obj_set_style_text_font(page,font,0);
    title=lv_label_create(page);
    lv_obj_set_pos(title,6,39);lv_label_set_long_mode(title,LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    body=lv_obj_create(page);lv_obj_set_pos(body,0,68);
    lv_obj_set_style_border_width(body,0,0);lv_obj_set_style_pad_all(body,4,0);lv_obj_set_style_radius(body,0,0);
    for(int i=0;i<10;i++){
      buttons[i]=lv_button_create(body);
      lv_obj_set_style_shadow_width(buttons[i],0,0);lv_obj_set_style_pad_all(buttons[i],6,0);
      auto *label=lv_label_create(buttons[i]);lv_label_set_long_mode(label,LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
      context[i]={this,i};
      lv_obj_add_event_cb(buttons[i],[](lv_event_t *e){
        auto *c=static_cast<Context *>(lv_event_get_user_data(e));
        c->self->model.focus=c->index;c->self->activate();
      },LV_EVENT_CLICKED,&context[i]);
    }
  }
  void begin(){model.begin();}
  void activate(){if(model.activate() && on_close)on_close();else render(dark,mono);}
  void move(int delta){model.move(delta);render(dark,mono);}
  void adjust(int delta){model.adjust(delta);render(dark,mono);}
  bool back(){bool close=model.back();if(!close)render(dark,mono);return close;}
  void render(bool is_dark,bool monochrome){
    if(!page)return;
    dark=is_dark;mono=monochrome;
    int w=lv_display_get_horizontal_resolution(lv_display_get_default());
    int h=lv_display_get_vertical_resolution(lv_display_get_default());
    auto fg=lv_color_hex(dark?0xFFFFFF:0),bg=lv_color_hex(dark?0:0xFFFFFF);
    for(auto *o:{page,body}){lv_obj_set_style_bg_color(o,bg,0);lv_obj_set_style_text_color(o,fg,0);}
    lv_obj_set_style_pad_right(body,0,LV_PART_SCROLLBAR);
    lv_obj_set_style_width(body,3,LV_PART_SCROLLBAR);
    lv_obj_set_width(title,w-12);lv_label_set_text(title,model.title().c_str());lv_obj_set_size(body,w,h-68);
    for(int i=0;i<10;i++){
      auto *b=buttons[i];
      if(i>=model.total()){lv_obj_add_flag(b,LV_OBJ_FLAG_HIDDEN);continue;}
      lv_obj_remove_flag(b,LV_OBJ_FLAG_HIDDEN);lv_obj_set_pos(b,0,i*(nabla_style::borders?46:34));lv_obj_set_size(b,w-8,nabla_style::borders?40:32);
      lv_obj_set_style_pad_top(b,nabla_style::borders?8:4,0);
      lv_obj_set_style_pad_bottom(b,nabla_style::borders?8:4,0);
      nabla_style::control(b,i==model.focus,dark,mono);
      auto *label=lv_obj_get_child(b,0);
      lv_obj_set_width(label,w-28);lv_label_set_text(label,model.row(i).c_str());lv_obj_center(label);
    }
    lv_obj_update_layout(body);lv_obj_scroll_to_view(buttons[model.focus],LV_ANIM_OFF);
  }
};
inline RegularForm regular_form;
}
