#pragma once
#include "model.h"
#include "compact_wifi.h"
#include "geometry.h"
#include "esphome/components/display/display.h"
#include "esphome/components/font/font.h"
namespace nabla {
class CompactShell {
 public:
  CompactMenu menu;
  nabla_forms::CompactWifi wifi;
  nabla_forms::Controller forms;
  int form_focus=-1,form_view=-1,wifi_focus=-1,wifi_stage=-1;
  uint32_t form_since=0,wifi_since=0;
  bool in_forms() const {return nodes[menu.current].action==4;}
  void adjust(int delta){if(in_forms())forms.adjust(delta);}
  bool in_wifi() const {return nodes[menu.current].action==3;}
  void move(int delta) {if(in_wifi())wifi.move(delta);else if(in_forms())forms.move(delta);else menu.move(delta);}
  void activate() {
    if(in_wifi()) {if(wifi.activate(esphome::millis())) menu.back();}
    else if(in_forms()){if(forms.activate())menu.back();}
    else {menu.activate();if(in_wifi())wifi.clear();if(in_forms())forms.begin();}
  }
  void back() {if(in_wifi()) {if(wifi.back())menu.back();}else if(in_forms()){if(forms.back())menu.back();}else menu.back();}
  void home() {wifi.clear();forms.discard();menu.home();}
  void render_wifi(esphome::display::Display &d, esphome::font::Font *small,
                   esphome::font::Font *body, esphome::font::Font *large) {
    using namespace esphome;using namespace nabla_forms;
    wifi.flow.tick(millis());
    if(wifi_focus!=wifi.focus || wifi_stage!=int(wifi.flow.stage)){
      wifi_focus=wifi.focus;wifi_stage=int(wifi.flow.stage);wifi_since=millis();
    }
    Color fg=menu.dark?Color::WHITE:Color::BLACK,bg=menu.dark?Color::BLACK:Color::WHITE;
    d.fill(bg);
    auto t=wifi.text;
    if(wifi.editing) {
      d.print(2,0,small,fg,wifi.secret?t[1]:"SSID");
      std::string value=wifi.secret?std::string(wifi.draft.value.size(),'*'):wifi.draft.value;
      size_t pos=value.size()>18?value.size()-18:0;
      while(pos<value.size() && (static_cast<unsigned char>(value[pos])&0xC0)==0x80)++pos;
      d.start_clipping(2,12,126,25);
      d.print(2,12,body,fg,(value.substr(pos)+"_").c_str());d.end_clipping();
      const auto &keys=wifi.glyphs();
      std::string key=wifi.key<4?t[7+wifi.key]:keys[wifi.key-4];
      if(wifi.key==3)key="Aa / 12 / !?";
      if(key==" ")key="_";
      d.rectangle(0,27,128,25,fg);d.print(64,29,large,fg,display::TextAlign::TOP_CENTER,key.c_str());
      d.printf(2,54,small,fg,"%d/%d",wifi.key+1,wifi.total());
      d.printf(126,54,small,fg,display::TextAlign::TOP_RIGHT,"%d/%d",
        static_cast<int>(wifi.draft.value.size()),static_cast<int>(wifi.draft.limit));
      return;
    }
    int title=0;
    switch(wifi.flow.stage) {
      case Stage::SCANNING:title=11;break;case Stage::CONNECTING:title=12;break;
      case Stage::SCAN_ERROR:title=18;break;
      case Stage::SUCCESS:title=13;break;case Stage::FAILURE:title=14;break;
      case Stage::RESULTS:if(!wifi.flow.results())title=17;break;
      default:if(wifi.flow.error==Error::SSID)title=15;
        else if(wifi.flow.error==Error::PASSWORD)title=16;break;
    }
    d.print(2,0,small,fg,t[title]);
    int total=wifi.total();
    wifi.focus=std::min(wifi.focus,total-1);
    wifi.top=nabla::scroll_anchor(wifi.focus,wifi.top,3,total);
    for(int r=0;r<3;r++){
      int index=wifi.top+r;if(index>=total)break;
      int y=13+r*17;
      if(index==wifi.focus)d.rectangle(0,y,128,17,fg);
      d.start_clipping(3,y+1,124,y+15);
      auto label=wifi.row(index);int bx,by,bw,bh;
      d.get_text_bounds(0,0,label.c_str(),body,display::TextAlign::TOP_LEFT,&bx,&by,&bw,&bh);
      int offset=index==wifi.focus && bw>120?std::min(bw-120,std::max(0,int(((millis()-wifi_since)/100)%(bw-120+20))-10)):0;
      d.print(4-offset,y+2,body,fg,label.c_str());d.end_clipping();
    }
  }
  void render_forms(esphome::display::Display &d, esphome::font::Font *small,
                    esphome::font::Font *body,esphome::font::Font *large) {
    using namespace esphome;
    auto fg=menu.dark?Color::WHITE:Color::BLACK,bg=menu.dark?Color::BLACK:Color::WHITE;
    if(form_focus!=forms.focus || form_view!=int(forms.view)){
      form_focus=forms.focus;form_view=int(forms.view);form_since=millis();
    }
    d.fill(bg);
    if(forms.view==nabla_forms::FormView::NUMBER){
      auto value=std::to_string(forms.value);int bx,by,bw,bh;
      d.get_text_bounds(0,0,value.c_str(),small,display::TextAlign::TOP_LEFT,&bx,&by,&bw,&bh);
      d.start_clipping(2,0,122-bw,11);
      d.print(2,0,small,fg,forms.session.spec[forms.field].label);d.end_clipping();
      d.print(126,0,small,fg,display::TextAlign::TOP_RIGHT,value.c_str());
    }else{
      d.start_clipping(2,0,126,11);d.print(2,0,small,fg,forms.title().c_str());d.end_clipping();
    }
    int rows=menu.readable?1:3,rh=menu.readable?52:17;
    forms.top=nabla::scroll_anchor(forms.focus,forms.top,rows,forms.total());
    for(int r=0;r<rows;r++){
      int index=forms.top+r;if(index>=forms.total())break;
      int y=12+r*rh;bool selected=index==forms.focus;
      if(selected)d.rectangle(0,y,128,rh,fg);
      auto *font=menu.readable?large:body;const auto label=forms.row(index);
      int bx,by,bw,bh;
      d.get_text_bounds(0,0,label.c_str(),font,display::TextAlign::TOP_LEFT,&bx,&by,&bw,&bh);
      int offset=selected && bw>120?int(((millis()-form_since)/100)%(bw-120+20)):0;
      offset=std::clamp(offset-10,0,std::max(0,bw-120));
      d.start_clipping(3,y+1,124,y+rh-2);
      d.print(4-offset,y+(rh-bh)/2,font,fg,label.c_str());d.end_clipping();
    }
  }
  bool app_footer = false;
  int last_node = -1, last_focus = -1;
  bool last_readable = false;
  uint32_t focus_since = 0;
  void render(esphome::display::Display &d, esphome::font::Font *small,
              esphome::font::Font *body, esphome::font::Font *large,
              const char *back_text, const char *pending) {
    using namespace esphome;
    if(!in_forms())form_focus=-1;
    if(!in_wifi())wifi_focus=-1;
    if(in_forms()){render_forms(d,small,body,large);return;}
    if(in_wifi()){render_wifi(d,small,body,large);return;}
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
        const std::string row_text = row_title(child(menu.current, index));
        const char *title = row_text.c_str();
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
      const std::string detail_text = nabla::detail(menu.current);
      const char *detail = nodes[menu.current].action == 3 ? pending : detail_text.c_str();
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
    if(in_forms()){
      if(y<12){back();return;}
      int rh=menu.readable?52:17,index=forms.top+(y-12)/rh;
      if(y<64 && index<forms.total()){forms.focus=index;activate();}return;
    }
    if(in_wifi()) {
      if(y<12) {back();return;}
      if(wifi.editing) {if(y>=27 && y<52)activate();else if(y>=52)move(x<64?-1:1);return;}
      int row=(y-13)/17;
      if(y>=13 && row<3 && wifi.top+row<wifi.total()){wifi.focus=wifi.top+row;activate();}
      return;
    }
    if (y < 12) { if (x < 16) menu.touch_option(children(menu.current)); return; }
    if (!children(menu.current)) { if (y >= 64-(app_footer ? 12 : 0)-14 && y < 64-(app_footer ? 12 : 0)) menu.back(); return; }
    auto g = Geometry::compact(menu.readable, menu.current == 0 || app_footer);
    if (y >= 64-g.footer) return;
    int row = (y-12)/g.row_height;
    if (row < g.rows) {menu.touch_option(menu.top + row);if(in_wifi())wifi.clear();if(in_forms())forms.begin();}
  }
};
inline CompactShell compact_shell;
}
