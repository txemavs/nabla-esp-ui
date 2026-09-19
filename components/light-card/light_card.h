// Reusable LVGL light-card rendering and callbacks; keeps light presentation independent of its data adapter.

#pragma once
#include "lvgl.h"
#include <functional>
#include <string>
#include <algorithm>
namespace nabla_light {
struct State { bool on=false; int brightness=0; bool dimmable=false; bool available=true; };
struct Words { const char *on; const char *off; const char *unavailable; const char *brightness; const char *apply; const char *cancel; };
class Editor {
 public:
  lv_obj_t *overlay=nullptr, *slider=nullptr, *value=nullptr, *buttons[3]{};
  State draft;
  Words words{};
  std::function<void(State)> commit;
  int focus=0;
  bool editing=false;
  bool active() const { return overlay!=nullptr; }
  void close() { if(overlay) lv_obj_delete(overlay); overlay=nullptr; }
  void refresh() {
    if(!overlay)return;
    lv_label_set_text_fmt(value,"%s - %d%%",draft.on?words.on:words.off,draft.brightness);
    lv_slider_set_value(slider,draft.brightness,LV_ANIM_OFF);
    for(int i=0;i<3;i++) {
      lv_obj_set_style_border_width(buttons[i],focus==i+1?2:0,0);
      lv_obj_set_style_border_color(buttons[i],lv_color_hex(0xF4C45E),0);
    }
    lv_obj_set_style_outline_width(slider,focus==0?2:0,0);
  }
  void move(int d) {
    if(editing && focus==0) { draft.brightness=std::clamp(draft.brightness+d*5,0,100);draft.on=draft.brightness>0; }
    else focus=(focus+d+4)%4;
    refresh();
  }
  void activate() {
    if(focus==0)editing=!editing;
    else if(focus==1){draft.on=!draft.on;if(draft.on && draft.brightness==0)draft.brightness=100;}
    else if(focus==2){auto s=draft;auto cb=commit;close();if(cb)cb(s);return;}
    else {close();return;}
    refresh();
  }
  void open(const char *title,State state,Words labels,const lv_font_t *font,bool dark,std::function<void(State)> cb) {
    if(!state.available)return;
    close();draft=state;words=labels;commit=cb;focus=0;editing=false;
    overlay=lv_obj_create(lv_layer_top());
    lv_obj_set_size(overlay,LV_PCT(100),LV_PCT(100));lv_obj_center(overlay);
    lv_obj_set_style_radius(overlay,0,0);lv_obj_remove_flag(overlay,LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(overlay,lv_color_hex(dark?0x171717:0xFFFFFF),0);
    lv_obj_set_style_text_color(overlay,lv_color_hex(dark?0xFFFFFF:0x000000),0);
    lv_obj_set_style_text_font(overlay,font,0);

    lv_obj_t *heading=lv_label_create(overlay);lv_label_set_text(heading,title);lv_obj_align(heading,LV_ALIGN_TOP_MID,0,4);
    value=lv_label_create(overlay);lv_obj_align(value,LV_ALIGN_CENTER,0,-35);
    slider=lv_slider_create(overlay);lv_slider_set_range(slider,0,100);
    lv_obj_set_width(slider,LV_PCT(90));lv_obj_set_height(slider,20);
    lv_obj_align(slider,LV_ALIGN_CENTER,0,0);
    lv_obj_set_style_bg_color(slider,lv_color_hex(0xF4C45E),LV_PART_INDICATOR);
    lv_obj_set_style_outline_color(slider,lv_color_hex(0xF4C45E),0);
    lv_obj_add_event_cb(slider,[](lv_event_t *e){auto *self=static_cast<Editor*>(lv_event_get_user_data(e));self->draft.brightness=lv_slider_get_value(self->slider);self->draft.on=self->draft.brightness>0;self->refresh();},LV_EVENT_VALUE_CHANGED,this);
    lv_obj_t *row=lv_obj_create(overlay);lv_obj_set_size(row,LV_PCT(100),48);lv_obj_align(row,LV_ALIGN_BOTTOM_MID,0,0);
    lv_obj_set_style_pad_all(row,0,0);lv_obj_set_style_border_width(row,0,0);lv_obj_set_style_bg_opa(row,LV_OPA_TRANSP,0);lv_obj_set_flex_flow(row,LV_FLEX_FLOW_ROW);
    const char *titles[]={words.on,words.apply,words.cancel};
    for(int i=0;i<3;i++){
      buttons[i]=lv_button_create(row);lv_obj_set_flex_grow(buttons[i],1);
      lv_obj_set_height(buttons[i],42);lv_obj_t *t=lv_label_create(buttons[i]);lv_label_set_text(t,titles[i]);lv_obj_center(t);
      lv_obj_add_event_cb(buttons[i],[](lv_event_t *e){auto *self=static_cast<Editor*>(lv_event_get_user_data(e));for(int j=0;j<3;j++)if(lv_event_get_target_obj(e)==self->buttons[j])self->focus=j+1;self->activate();},LV_EVENT_CLICKED,this);
    }
    refresh();
  }
};
inline Editor editor;
inline void card(lv_obj_t *button,lv_obj_t *label,const char *title,State state,Words words,bool dark,bool focused,int width,int height,const lv_font_t *icon_font=nullptr) {
  state.brightness=std::clamp(state.brightness,0,100);
  const bool lit=state.available && state.on;
  const auto accent=lv_color_hex(!state.available?0x808080:lit?0xE8B84B:(dark?0x499EFF:0x0055BB));
  lv_obj_set_size(button,width,height);
  lv_obj_set_style_pad_all(button,4,0);
  lv_obj_set_width(label,width-10);
  lv_obj_set_style_text_align(label,LV_TEXT_ALIGN_CENTER,0);
  lv_obj_align(label,LV_ALIGN_CENTER,0,25);
  if(icon_font) {
    lv_obj_t *icon=lv_obj_get_child_count(button)>1?lv_obj_get_child(button,1):lv_label_create(button);
    lv_obj_remove_flag(icon,LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(icon,"\xEF\x83\xAB");
    lv_obj_set_style_text_font(icon,icon_font,0);
    lv_obj_align(icon,LV_ALIGN_CENTER,0,-25);
    lv_obj_set_style_text_color(icon,accent,0);
    lv_obj_t *bar=lv_obj_get_child_count(button)>2?lv_obj_get_child(button,2):lv_bar_create(button);
    if(state.dimmable) {
      lv_obj_remove_flag(bar,LV_OBJ_FLAG_HIDDEN);
      lv_obj_set_size(bar,std::max(20,width-36),4);
      lv_obj_align(bar,LV_ALIGN_CENTER,0,0);
      lv_bar_set_range(bar,0,100);
      lv_bar_set_value(bar,lit?state.brightness:0,LV_ANIM_OFF);
      lv_obj_set_style_bg_color(bar,lv_color_hex(dark?0x383838:0xDDDDDD),0);
      lv_obj_set_style_bg_color(bar,accent,LV_PART_INDICATOR);
    } else lv_obj_add_flag(bar,LV_OBJ_FLAG_HIDDEN);
  }
  lv_label_set_long_mode(label,LV_LABEL_LONG_WRAP);
  std::string text=title;
  text+="\n";
  text+=!state.available?words.unavailable:state.on?words.on:words.off;
  if(state.dimmable && lit)text+=" "+std::to_string(state.brightness)+"%";
  lv_label_set_text(label,text.c_str());
  lv_obj_set_style_bg_color(button,lv_color_hex(lit?(dark?0x42361D:0xFFF0C6):(dark?0x181818:0xF1F1F1)),0);
  lv_obj_set_style_text_color(label,!state.available?lv_color_hex(0x808080):lv_color_hex(dark?0xFFFFFF:0x000000),0);
  lv_obj_set_style_border_color(button,lv_color_hex(focused?(dark?0xFFFFFF:0x000000):(dark?0x383838:0xDDDDDD)),0);
  lv_obj_set_style_border_width(button,focused?2:1,0);
}
}
