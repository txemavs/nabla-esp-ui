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
  esphome::font::Font *icons=nullptr;
  bool color_icons=false, root_tiles=false, single_icon_mode=false, list_footer=false, middle_scroll=false;
  std::function<int(int)> icon_color;
  std::function<bool(int)> custom_move;
  std::function<bool()> custom_activate,custom_back;
  std::function<bool(esphome::display::Display &,esphome::font::Font *,esphome::font::Font *)> custom_render;
  std::function<std::string(int)> custom_detail;
  int detail_scroll=0, detail_max=0, detail_node=-1;

  nabla_forms::CompactWifi wifi;
  nabla_forms::Controller forms;
  int form_focus=-1,form_view=-1,wifi_focus=-1,wifi_stage=-1;
  uint32_t form_since=0,wifi_since=0;
  bool in_forms() const {return nodes[menu.current].action==4;}
  void adjust(int delta){if(in_forms())forms.adjust(delta);}
  bool in_wifi() const {return nodes[menu.current].action==3;}
  void move(int delta) {if(custom_move && custom_move(delta))return;if(!children(menu.current) && !in_wifi() && !in_forms()){detail_scroll=std::clamp(detail_scroll+delta,0,detail_max);return;}if(in_wifi())wifi.move(delta);else if(in_forms())forms.move(delta);else menu.move(delta);}
  void activate() {
    if(custom_activate && custom_activate())return;
    if(in_wifi()) {if(wifi.activate((esphome::millis)())) menu.back();}
    else if(in_forms()){if(forms.activate())menu.back();}
    else {menu.activate();if(in_wifi())wifi.clear();if(in_forms())forms.begin();}
  }
  void back() {if(custom_back && custom_back())return;if(in_wifi()) {if(wifi.back())menu.back();}else if(in_forms()){if(forms.back())menu.back();}else menu.back();}
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
      if(menu.borders)d.rectangle(0,27,128,25,fg);else d.filled_rectangle(0,27,128,25,fg);d.print(64,29,large,menu.borders?fg:bg,display::TextAlign::TOP_CENTER,key.c_str());
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
      if(index==wifi.focus) { if(menu.borders)d.rectangle(0,y,128,17,fg); else d.filled_rectangle(0,y,128,17,fg); }
      d.start_clipping(3,y+1,124,y+15);
      auto label=wifi.row(index);int bx,by,bw,bh;
      d.get_text_bounds(0,0,label.c_str(),body,display::TextAlign::TOP_LEFT,&bx,&by,&bw,&bh);
      int offset=index==wifi.focus && bw>120?std::min(bw-120,std::max(0,int(((millis()-wifi_since)/100)%(bw-120+20))-10)):0;
      d.print(4-offset,y+2,body,index==wifi.focus && !menu.borders?bg:fg,label.c_str());d.end_clipping();
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
      if(selected) { if(menu.borders)d.rectangle(0,y,128,rh,fg); else d.filled_rectangle(0,y,128,rh,fg); }
      auto *font=menu.readable?large:body;const auto label=forms.row(index);
      int bx,by,bw,bh;
      d.get_text_bounds(0,0,label.c_str(),font,display::TextAlign::TOP_LEFT,&bx,&by,&bw,&bh);
      int offset=selected && bw>120?int(((millis()-form_since)/100)%(bw-120+20)):0;
      offset=std::clamp(offset-10,0,std::max(0,bw-120));
      d.start_clipping(3,y+1,124,y+rh-2);
      d.print(4-offset,y+(rh-bh)/2,font,selected && !menu.borders?bg:fg,label.c_str());d.end_clipping();
    }
  }
  int bar_height = 12, display_width = 128, display_height = 64;
  bool app_footer = false;
  int last_node = -1, last_focus = -1;
  bool last_readable = false;
  uint32_t focus_since = 0;
  void render(esphome::display::Display &d, esphome::font::Font *small,
              esphome::font::Font *body, esphome::font::Font *large,
              const char *back_text, const char *pending) {
    using namespace esphome;
    if(custom_render && custom_render(d,small,body))return;
    if(!in_forms())form_focus=-1;
    if(!in_wifi())wifi_focus=-1;
    if(in_forms()){render_forms(d,small,body,large);return;}
    if(in_wifi()){render_wifi(d,small,body,large);return;}
    if (last_node != menu.current || last_focus != menu.focus || last_readable != menu.readable) {
      last_node = menu.current; last_focus = menu.focus; last_readable = menu.readable;
      focus_since = millis();
    }
    display_width=d.get_width();display_height=d.get_height();
    // Footer: readable (icon) mode keeps footer; list mode at root uses list_footer flag.
    bool footer_visible = menu.readable ? (menu.current == 0 || app_footer)
                                        : (menu.current == 0 ? list_footer : app_footer);
    auto g = Geometry::compact(menu.readable, footer_visible,
      display_width,display_height,bar_height,menu.list_rows);
    Color fg = menu.dark ? Color::WHITE : Color::BLACK;
    Color bg = menu.dark ? Color::BLACK : Color::WHITE;
    d.fill(bg);
    bool header_focus = menu.focus == children(menu.current);
    if(header_focus && !menu.borders) d.filled_rectangle(0,0,15,g.header,fg);
    auto mark_color=header_focus && !menu.borders?bg:fg;
    // Equilateral triangle, fixed center. Up on parent focus, down otherwise.
    if (header_focus && menu.current)
      d.triangle(2, 10, 12, 10, 7, 1, mark_color);
    else d.triangle(2, 1, 12, 1, 7, 10, mark_color);
    if (header_focus && !menu.current && menu.borders) d.rectangle(0, 0, 15, g.header, fg);
    d.start_clipping(16, 0, g.width-1, g.header-1);
    d.print(16, 0, small, fg, nodes[menu.current].title);
    d.end_clipping();
    const int n = children(menu.current);
    if(root_tiles && icons && menu.current==0 && n && (!single_icon_mode || menu.readable)) {
      int cells=menu.readable?1:4;
      int first=menu.focus<n?(menu.focus/cells)*cells:(menu.top/cells)*cells;
      int cols=menu.readable?1:2,rows=menu.readable?1:2;
      int cw=g.width/cols,ch=g.content_height()/rows;
      for(int i=0;i<cells && first+i<n;i++){
        int node=child(0,first+i),x=(i%cols)*cw,y=g.header+(i/cols)*ch;
        bool selected=menu.focus==first+i;
        auto ink=selected&&!menu.borders?bg:fg;
        if(selected){if(menu.borders)d.rectangle(x,y,cw,ch,fg);else d.filled_rectangle(x,y,cw,ch,fg);}
        auto packed=menu.dark?nodes[node].icon_dark:nodes[node].icon_light;
        auto color=color_icons?Color((packed>>16)&255,(packed>>8)&255,packed&255):ink;
        if(selected&&!menu.borders)color=ink;
        // Measure label height for vertical centering.
        int bx,by,bw,bh;
        d.get_text_bounds(0,0,nodes[node].title,small,display::TextAlign::TOP_LEFT,&bx,&by,&bw,&bh);
        // For single_icon_mode, vertically center icon+label in content area.
        int icon_h = icons->get_height();
        int total_h = icon_h + 2 + bh;  // icon + gap + label
        int base_y = y + (ch - total_h) / 2;
        int icon_y = base_y + icon_h / 2;
        int label_y = base_y + icon_h + 2;
        d.print(x+cw/2,icon_y,icons,color,display::TextAlign::CENTER,nodes[node].icon);
        d.start_clipping(x+3,y,x+cw-4,y+ch-2);
        if(bw<=cw-6)d.print(x+cw/2,label_y,small,ink,display::TextAlign::TOP_CENTER,nodes[node].title);
        else {
          int offset=selected?std::min(bw-cw+6,std::max(0,int(((millis()-focus_since)/100)%(bw-cw+26))-10)):0;
          d.print(x+3-offset,label_y,small,ink,nodes[node].title);
        }
        d.end_clipping();
      }
    } else if (n) {
      // Use middle-scroll if enabled (selection stays in middle row when possible).
      int list_top = middle_scroll ? scroll_anchor_middle(menu.focus, g.rows, n) : menu.top;
      for (int r = 0; r < g.rows; ++r) {
        int index = list_top + r;
        if (index >= n) break;
        int y = g.header + r * g.row_height;
        bool selected = menu.focus == index;
        if (selected) { if(menu.borders) d.rectangle(0,y,g.width,g.row_height,fg); else d.filled_rectangle(0,y,g.width,g.row_height,fg); }
        auto *font = menu.readable ? large : body;
        int row_node=child(menu.current,index);
        int left=4;
        if(icons && nodes[row_node].icon[0] && !single_icon_mode){
          left=32;int packed=icon_color?icon_color(row_node):(menu.dark?nodes[row_node].icon_dark:nodes[row_node].icon_light);
          auto ink=color_icons?Color((packed>>16)&255,(packed>>8)&255,packed&255):fg;
          if(selected&&!menu.borders)ink=bg;
          d.print(16,y+g.row_height/2,icons,ink,display::TextAlign::CENTER,nodes[row_node].icon);
        }
        const std::string row_text = row_title(row_node, menu.dark, menu.font_family, menu.borders);
        const char *title = row_text.c_str();
        int bx, by, bw, bh;
        d.get_text_bounds(0, 0, title, font, display::TextAlign::TOP_LEFT, &bx,&by,&bw,&bh);
        // Bounded marquee exposes complete long labels without changing focus.
        int offset = 0;
        if (selected && bw > g.width-left-4) {
          int range = bw - (g.width-left-4);
          int phase = ((millis()-focus_since)/100) % (2*range + 20);
          offset = std::min(range, std::max(0, phase-10));
          if (phase > range+10) offset = std::max(0, 2*range+10-phase);
        }
        d.start_clipping(left-1, y+1, g.width-4, y+g.row_height-2);
        d.print(left-offset, y+(g.row_height-bh)/2, font, selected && !menu.borders ? bg : fg, title);
        d.end_clipping();
      }
    } else {
      if(detail_node!=menu.current){detail_node=menu.current;detail_scroll=0;}
      std::string text=custom_detail?custom_detail(menu.current):"";
      if(text.empty())text=nabla::detail(menu.current);
      if(nodes[menu.current].action==3)text=pending;
      text=text.substr(0,900);
      std::vector<std::string> lines;
      std::string line;
      for(size_t pos=0;pos<text.size();){
        size_t next=pos+1;
        while(next<text.size() && (static_cast<unsigned char>(text[next])&0xc0)==0x80)++next;
        auto glyph=text.substr(pos,next-pos);pos=next;
        if(glyph=="\n"){lines.push_back(line);line.clear();continue;}
        int bx,by,bw,bh;
        auto candidate=line+glyph;
        d.get_text_bounds(0,0,candidate.c_str(),body,display::TextAlign::TOP_LEFT,&bx,&by,&bw,&bh);
        if(bw>g.width-6 && !line.empty()){lines.push_back(line);line.clear();}
        line+=glyph;
      }
      lines.push_back(line);
      int visible=std::max(1,(g.content_height()-g.header-4)/body->get_height());
      detail_max=std::max(0,int(lines.size())-visible);
      detail_scroll=std::clamp(detail_scroll,0,detail_max);
      d.start_clipping(2,g.header+2,g.width-2,g.height-g.footer-g.header-2);
      for(int i=0;i<visible && detail_scroll+i<int(lines.size());i++)
        d.print(2,g.header+2+i*body->get_height(),body,fg,lines[detail_scroll+i].c_str());
      d.end_clipping();
      // Header is the semantic Back target; provide an obvious touch-sized row.
      if(menu.borders)d.rectangle(0,g.height-g.footer-g.header,g.width,g.header,fg);else d.filled_rectangle(0,g.height-g.footer-g.header,g.width,g.header,fg);
      d.print(4, g.height-g.footer-g.header, body, menu.borders?fg:bg, back_text);
    }
    if (g.footer) {
      d.print(2, g.height-g.footer+2, small, fg, "NABLA");
      if (n) d.printf(g.width-2, g.height-g.footer+2, small, fg, display::TextAlign::TOP_RIGHT,
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
    if (y < bar_height) { if (x < 16) menu.touch_option(children(menu.current)); return; }
    if (!children(menu.current)) { if (y >= display_height-(app_footer ? bar_height : 0)-bar_height && y < display_height-(app_footer ? bar_height : 0)) menu.back(); return; }
    auto g = Geometry::compact(menu.readable, menu.current == 0 || app_footer,display_width,display_height,bar_height,menu.list_rows);
    if (y >= display_height-g.footer) return;
    if(root_tiles && icons && menu.current==0 && (!single_icon_mode || menu.readable)){
      int cells=menu.readable?1:4,cols=menu.readable?1:2;
      int first=menu.focus<children(0)?menu.focus/cells*cells:menu.top/cells*cells;
      int index=first+(y-bar_height)/(g.content_height()/(menu.readable?1:2))*cols+x/(g.width/cols);
      if(index<children(0))menu.touch_option(index);
      return;
    }
    int row = (y-bar_height)/g.row_height;
    if (row < g.rows) {menu.touch_option(menu.top + row);if(in_wifi())wifi.clear();if(in_forms())forms.begin();}
  }
};
inline CompactShell compact_shell;
}
