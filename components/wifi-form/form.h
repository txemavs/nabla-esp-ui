#pragma once
#include "lvgl.h"
#include "esphome/components/nabla_navigation/control_style.h"
#include "wifi_flow.h"
#include <cstring>
namespace nabla_wifi_form {
// Bounded, local editor state. Credentials live only in the textarea.
struct Editor {
  lv_obj_t *page{}, *ssid{}, *password{}, *keyboard{}, *status{}, *apply{}, *cancel{};
  lv_obj_t *scan{}, *open_box{}, *scan_panel{}, *scan_title{};
  lv_obj_t *scan_rows[5]{};
  const char *const *words{};
  const char *const *real_words{};
  bool reset_confirm=false;
  nabla_forms::WifiFlow flow;
  int scan_index=0, scan_top=0;
  bool scan_picker=false;
  struct RowContext {Editor *self;int index;} row_context[5];
  void setup_scan() {
    if(scan_panel)return;
    scan_panel=lv_obj_create(page);
    lv_obj_set_style_radius(scan_panel,0,0);lv_obj_set_style_pad_all(scan_panel,4,0);
    lv_obj_set_style_border_width(scan_panel,0,0);
    scan_title=lv_label_create(scan_panel);
    for(int i=0;i<5;i++){
      scan_rows[i]=lv_button_create(scan_panel);
      lv_obj_set_style_shadow_width(scan_rows[i],0,0);
      lv_label_create(scan_rows[i]);row_context[i]={this,i};
      lv_obj_add_event_cb(scan_rows[i],[](lv_event_t *e){
        auto *ctx=static_cast<RowContext *>(lv_event_get_user_data(e));
        ctx->self->select_scan(ctx->self->scan_top+ctx->index);
      },LV_EVENT_CLICKED,&row_context[i]);
    }
    lv_obj_add_flag(scan_panel,LV_OBJ_FLAG_HIDDEN);
  }
  bool scanning() const {
    return reset_confirm || scan_picker || flow.stage==nabla_forms::Stage::SCANNING || flow.stage==nabla_forms::Stage::RESULTS || flow.stage==nabla_forms::Stage::SCAN_ERROR;
  }
  int scan_count() const {
    return reset_confirm?2:scan_picker?(flow.real()?3:4):flow.stage==nabla_forms::Stage::RESULTS?flow.results()+1:1;
  }
  void start_scan() {scan_picker=true;scan_index=scan_top=0;poll();}
  void select_scan(int index) {
    if(reset_confirm){
      reset_confirm=false;
      if(index==1 && nabla_forms::WifiFlow::backend){
        bool ok=nabla_forms::WifiFlow::backend->forget();
        clear();
        flow.stage=ok?nabla_forms::Stage::EDIT:nabla_forms::Stage::FAILURE;
      }
      scan_index=scan_top=0;
    }else if(scan_picker && flow.real()){
      scan_picker=false;scan_index=scan_top=0;
      if(index==0)flow.scan(esphome::millis());
      else if(index==1)reset_confirm=true;
    }else if(scan_picker){
      scan_picker=false;
      if(index<3)flow.scan(esphome::millis(),static_cast<nabla_forms::ScanMode>(index));
      scan_index=scan_top=0;
    }else if(flow.stage==nabla_forms::Stage::RESULTS && index<flow.results()){
      flow.choose(index);
      lv_textarea_set_text(ssid,flow.ssid.c_str());lv_textarea_set_text(password,"");
      if(flow.open)lv_obj_add_state(open_box,LV_STATE_CHECKED);
      else lv_obj_remove_state(open_box,LV_STATE_CHECKED);
      edit(!flow.open);
    }else {flow.cancel();focus=keys()+3;}
    poll();
  }
  void submit() {
    if(flow.stage==nabla_forms::Stage::CONNECTING || flow.stage==nabla_forms::Stage::SUCCESS || scanning())return;
    flow.ssid=lv_textarea_get_text(ssid);
    flow.password=lv_textarea_get_text(password);
    flow.open=lv_obj_has_state(open_box,LV_STATE_CHECKED);
    if(flow.connect(esphome::millis())){
      lv_textarea_set_text(password,"");focus=keys()+5;highlight();
    }
    else {
      edit(flow.error==nabla_forms::Error::PASSWORD);
      flow.wipe_password();
    }
    poll();
  }
  void poll() {
    if(!page || !ssid || !password || !status)return;
    using namespace nabla_forms;
    flow.tick(esphome::millis());
    if((flow.stage==Stage::SUCCESS || flow.stage==Stage::FAILURE) &&
       flow.open!=lv_obj_has_state(open_box,LV_STATE_CHECKED))flow.cancel();
    auto fg=lv_color_hex(dark_theme?0xFFFFFF:0),bg=lv_color_hex(dark_theme?0:0xFFFFFF);
    bool busy=flow.stage==Stage::CONNECTING;
    for(auto *o:{ssid,password,keyboard,apply,scan,open_box}){
      if(busy)lv_obj_add_state(o,LV_STATE_DISABLED);else lv_obj_remove_state(o,LV_STATE_DISABLED);
    }
    int title=flow.error==Error::SSID?15:flow.error==Error::PASSWORD?16:
      flow.stage==Stage::CONNECTING?12:flow.stage==Stage::SUCCESS?13:
      flow.stage==Stage::FAILURE?14:0;
    if(words) {
      const char *caption=words[title];
      if(flow.real() && real_words){
        if(title==0)caption=real_words[0];
        if(title==13)caption=real_words[1];
        if(title==14)caption=real_words[2];
      }
      lv_label_set_text(status,caption);
    }
    if(!scan_panel)return;
    if(!scanning()){lv_obj_add_flag(scan_panel,LV_OBJ_FLAG_HIDDEN);return;}
    lv_obj_remove_flag(scan_panel,LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(scan_panel);
    lv_obj_set_style_bg_color(scan_panel,bg,0);
    lv_obj_set_style_text_color(scan_panel,fg,0);
    int w=lv_display_get_horizontal_resolution(lv_display_get_default());
    int h=lv_display_get_vertical_resolution(lv_display_get_default())-40;
    lv_obj_set_pos(scan_panel,0,38);lv_obj_set_size(scan_panel,w,h);
    int heading=scan_picker?21:flow.stage==Stage::SCANNING?11:
      flow.stage==Stage::SCAN_ERROR?18:flow.stage==Stage::RESULTS && !flow.results()?17:0;
    lv_label_set_text(scan_title,reset_confirm?real_words[4]:flow.real() && heading==0?real_words[0]:words[heading]);
    int count=scan_count();
    scan_index=std::clamp(scan_index,0,count-1);
    scan_top=std::clamp(scan_top,0,std::max(0,count-5));
    if(scan_index<scan_top)scan_top=scan_index;
    if(scan_index>=scan_top+5)scan_top=scan_index-4;
    for(int i=0;i<5;i++){
      auto *b=scan_rows[i];
      int index=scan_top+i;
      if(index>=count){lv_obj_add_flag(b,LV_OBJ_FLAG_HIDDEN);continue;}
      lv_obj_remove_flag(b,LV_OBJ_FLAG_HIDDEN);
      lv_obj_set_pos(b,0,25+i*((h-30)/5));lv_obj_set_size(b,w-8,(h-30)/5-3);
      nabla_style::control(b,index==scan_index,dark_theme);
      auto *label=lv_obj_get_child(b,0);
      std::string caption=reset_confirm?(index==0?words[9]:real_words[5]):
        scan_picker && flow.real()?(index==0?words[3]:index==1?real_words[3]:words[6]):
        scan_picker?words[index==0?20:index==1?4:index==2?19:6]:
        flow.stage==Stage::RESULTS && index<flow.results()?flow.result_label(index):words[6];
      lv_obj_set_width(label,w-36);
      lv_label_set_long_mode(label,LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
      lv_label_set_text(label,caption.c_str());
      lv_obj_center(label);
    }
  }
  int focus = 0;
  bool editing_password = false;
  bool dark_theme = true;
  void clear() {
    flow.clear();scan_picker=false;reset_confirm=false;scan_index=scan_top=0;
    if(open_box)lv_obj_remove_state(open_box,LV_STATE_CHECKED);
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
      nabla_style::control(o,selected,dark_theme);
    };
    int n = keys();
    mark(ssid, focus == 0); mark(password, focus == 1);
    for(auto *field : {ssid,password}) {
      const bool selected=(field==ssid?focus==0:focus==1);
      auto ink=lv_color_hex(dark_theme?0xFFFFFF:0);
      if(!nabla_style::borders && selected) ink=lv_color_hex(dark_theme?0:0xFFFFFF);
      lv_obj_set_style_text_color(field,ink,LV_PART_TEXTAREA_PLACEHOLDER);
      lv_obj_set_style_bg_color(field,ink,LV_PART_CURSOR);
    }
    mark(apply, focus == n + 2); mark(scan, focus == n + 3); mark(open_box, focus == n + 4); mark(cancel, focus == n + 5);
    if (focus >= 2 && focus < n + 2) {
      lv_obj_add_state(keyboard, LV_STATE_FOCUSED);
      lv_buttonmatrix_set_selected_button(keyboard, focus - 2);
    } else {
      lv_obj_remove_state(keyboard, LV_STATE_FOCUSED);
      lv_buttonmatrix_set_selected_button(keyboard, LV_BUTTONMATRIX_BUTTON_NONE);
    }
  }
  void move(int delta) {
    if(scanning()){
      int n=scan_count();
      scan_index=(scan_index+delta%n+n)%n;poll();return;
    }
    if(flow.stage==nabla_forms::Stage::CONNECTING){focus=keys()+5;highlight();return;}
    int total = keys() + 6;
    focus = (focus + delta % total + total) % total;
    highlight();
  }
  void activate() {
    if(scanning()){select_scan(scan_index);return;}
    if(flow.stage==nabla_forms::Stage::CONNECTING){lv_obj_send_event(cancel,LV_EVENT_CLICKED,nullptr);return;}
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
      if(focus==n+4){
        if(lv_obj_has_state(open_box,LV_STATE_CHECKED))lv_obj_remove_state(open_box,LV_STATE_CHECKED);
        else lv_obj_add_state(open_box,LV_STATE_CHECKED);
      }else lv_obj_send_event(focus==n+2?apply:focus==n+3?scan:cancel,LV_EVENT_CLICKED,nullptr);
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
    lv_point_t toggle_text;
    lv_text_get_size(&toggle_text,lv_checkbox_get_text(open_box),
                    lv_obj_get_style_text_font(open_box,LV_PART_MAIN),0,0,LV_COORD_MAX,LV_TEXT_FLAG_NONE);
    const int toggle_width=toggle_text.x+32;
    lv_obj_set_style_pad_all(open_box,2,0);
    lv_obj_set_style_pad_column(open_box,4,0);
    lv_obj_set_pos(status,6,status_y);lv_obj_set_width(status,w-toggle_width-18);
    lv_obj_set_pos(open_box,w-toggle_width-6,status_y);lv_obj_set_width(open_box,toggle_width);
    int keyboard_y = status_y + 24;
    lv_obj_set_pos(keyboard, 4, keyboard_y);
    lv_obj_set_size(keyboard, w - 8, h - keyboard_y - 42);
    lv_obj_set_pos(apply, 6, h - 37); lv_obj_set_size(apply, (w - 24) / 3, 32);
    lv_obj_set_pos(scan,(w-24)/3+12,h-37);lv_obj_set_size(scan,(w-24)/3,32);
    lv_obj_set_pos(cancel, 2*(w-24)/3+18, h - 37);
    lv_obj_set_size(cancel, (w - 24) / 3, 32);
    auto bg = lv_color_hex(dark ? 0 : 0xFFFFFF);
    lv_obj_set_style_bg_color(page, bg, 0);
    lv_obj_set_style_bg_color(keyboard, bg, 0);
    lv_obj_set_style_text_color(status, lv_color_hex(dark ? 0xAAAAAA : 0x505050), 0);
    auto fg = lv_color_hex(dark ? 0xFFFFFF : 0x000000);
    for (auto *control : {ssid, password, apply, scan, open_box, cancel}) {
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
      bool inverse=!nabla_style::borders && (state & (LV_STATE_FOCUSED | LV_STATE_PRESSED));
      lv_obj_set_style_border_width(keyboard,nabla_style::borders ? ((state & (LV_STATE_FOCUSED | LV_STATE_PRESSED))?2:1) : 0,LV_PART_ITEMS | state);
      lv_obj_set_style_bg_color(keyboard,inverse?fg:bg,LV_PART_ITEMS | state);
      lv_obj_set_style_text_color(keyboard,inverse?bg:fg,LV_PART_ITEMS | state);
      lv_obj_set_style_border_color(keyboard,
          state & (LV_STATE_FOCUSED | LV_STATE_PRESSED) ? fg : nabla_style::idle(dark),
          LV_PART_ITEMS | state);
    }
    lv_obj_set_style_bg_color(open_box,bg,LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(open_box,fg,static_cast<lv_style_selector_t>(LV_PART_INDICATOR) | LV_STATE_CHECKED);
    lv_obj_set_style_border_color(open_box,nabla_style::idle(dark),LV_PART_INDICATOR);
    lv_obj_set_style_border_width(open_box,nabla_style::borders?1:0,LV_PART_INDICATOR);
    lv_obj_set_style_text_color(open_box,bg,static_cast<lv_style_selector_t>(LV_PART_INDICATOR) | LV_STATE_CHECKED);
    highlight();
  }
};
inline Editor editor;
}
