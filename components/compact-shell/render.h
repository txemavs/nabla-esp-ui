// Compact rendering shares selection decoration across menus, forms and detail actions.
// Compact display renderer; draws shared menu state within the active profile geometry.

#pragma once
#include "model.h"
#include "compact_wifi.h"
#include "geometry.h"
#include "esphome/components/display/display.h"
#include "esphome/components/font/font.h"
#include <cmath>
namespace nabla {
class CompactShell {
 public:
  CompactMenu menu;
  esphome::font::Font *icons=nullptr;
  esphome::font::Font *title_preview=nullptr, *title_ubuntu_preview=nullptr;
  bool color_icons=false, root_tiles=false, single_icon_mode=false, list_footer=false, middle_scroll=false;
  // Tiny profile: use play marker in Normal mode instead of box border.
  bool tiny_play_marker=false;
  // Animation state for triangle spin on boot and view toggle.
  uint32_t triangle_anim_start=0;
  bool triangle_animating=false;
  float triangle_angle=0;  // Current angle in degrees (0=down, 180=up).
  float parent_angle=0, parent_from=0, parent_target=0;
  uint32_t parent_since=0;
  int slide_from=-1, slide_to=-1, slide_direction=1;
  uint32_t slide_since=0;
  int triangle_spins=0;    // Number of complete depth turns.
  // Icon size reduction for tiny profile (pixels to subtract from compact_icon_size).
  int tiny_icon_reduce=0;

  void start_triangle_spin(int spins=1) {
    triangle_animating = true;
    triangle_anim_start = millis();
    triangle_spins = spins;
  }

  void init_tiny_callbacks() {
    menu.on_view_toggle = [this]() { start_triangle_spin(1); };
  }
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
  void move(int delta) {if(custom_move && custom_move(delta))return;if(!children(menu.current) && !in_wifi() && !in_forms()){detail_scroll=std::clamp(detail_scroll+delta,0,detail_max);return;}if(in_wifi())wifi.move(delta);else if(in_forms())forms.move(delta);else {
      int previous=menu.focus;
      menu.move(delta);
      if(single_icon_mode && menu.readable && menu.current==0 &&
         previous<children(0) && menu.focus<children(0) && previous!=menu.focus){
        slide_from=previous;slide_to=menu.focus;slide_direction=delta>0?1:-1;slide_since=millis();
      }else slide_from=-1;
    }}
  void activate() {
    if(custom_activate && custom_activate())return;
    if(in_wifi()) {if(wifi.activate((esphome::millis)())) menu.back();}
    else if(in_forms()){if(forms.activate())menu.back();}
    else {menu.activate();if(in_wifi())wifi.clear();if(in_forms())forms.begin();}
  }
  void back() {if(custom_back && custom_back())return;if(in_wifi()) {if(wifi.back())menu.back();}else if(in_forms()){if(forms.back())menu.back();}else menu.back();}
  void home() {wifi.clear();forms.discard();menu.home();}
  bool uses_play_marker() const { return tiny_play_marker && menu.borders; }
  int selection_inset() const { return uses_play_marker() ? 10 : 4; }
  void draw_selection(esphome::display::Display &d,int x,int y,int w,int h,
                      bool selected,esphome::Color fg) const {
    if(!selected)return;
    if(uses_play_marker()){
      int marker_h=std::min(8,std::max(2,h-4));
      int my=y+(h-marker_h)/2;
      d.filled_triangle(x+2,my,x+2,my+marker_h,x+7,my+marker_h/2,fg);
    }else if(menu.borders)d.rectangle(x,y,w,h,fg);
    else d.filled_rectangle(x,y,w,h,fg);
  }
  void draw_logo(esphome::display::Display &d,esphome::Color fg,bool header_focus) {
    // Parent navigation rotates 60 degrees in-plane; view changes spin in depth.
    float target=(header_focus && menu.current)?60.0f:0.0f;
    uint32_t now=millis();
    float t=std::min(1.0f,float(now-parent_since)/180.0f);
    parent_angle=parent_from+(parent_target-parent_from)*t;
    if(target!=parent_target){parent_from=parent_angle;parent_target=target;parent_since=now;}
    float depth_scale=1.0f;
    if(triangle_animating){
      float turn=float(now-triangle_anim_start)/1000.0f;
      if(turn>=triangle_spins)triangle_animating=false;
      else depth_scale=cosf(turn*6.2831853f);
    }
    // A depth spin foreshortens x only, keeping the bottom tip fixed.
    auto vertex=[&](float offset)->std::pair<int,int>{
      float a=(90.0f+offset+parent_angle)*3.14159265f/180.0f;
      return {int(lroundf(7.0f+5.0f*cosf(a)*depth_scale)),
              int(lroundf(5.5f+5.0f*sinf(a)))};
    };
    auto v0=vertex(0),v1=vertex(120),v2=vertex(240);
    if(header_focus || triangle_animating)
      d.filled_triangle(v0.first,v0.second,v1.first,v1.second,v2.first,v2.second,fg);
    else d.triangle(v0.first,v0.second,v1.first,v1.second,v2.first,v2.second,fg);
  }
  void draw_form_header(esphome::display::Display &d,esphome::font::Font *font,
                        esphome::Color fg,const char *title,bool focused=false) {
    draw_logo(d,fg,focused);
    d.start_clipping(16,0,127,bar_height-1);
    d.print(16,0,font,fg,title);
    d.end_clipping();
  }
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
    const int width=d.get_width(), height=d.get_height();
    if(wifi.editing) {
      const int key_height=std::max(20,body->get_height()+4);
      const int key_top=height-key_height;
      const int palette_y=std::max(bar_height+body->get_height()+2,key_top-small->get_height()-3);
      const int center=width/2, step=std::max(20,width/5);
      draw_form_header(d,small,fg,wifi.secret?t[1]:"SSID",wifi.header_selected());
      std::string value=wifi.secret?std::string(wifi.draft.value.size(),'*'):wifi.draft.value;
      size_t pos=value.size()>18?value.size()-18:0;
      while(pos<value.size() && (static_cast<unsigned char>(value[pos])&0xC0)==0x80)++pos;
      d.start_clipping(2,bar_height,width-3,palette_y-1);
      d.print(2,bar_height+1,body,fg,(value.substr(pos)+"_").c_str());d.end_clipping();
      static const char *palettes[]={"abc","ABC","123","!?#"};
      d.print(2,palette_y,small,fg,palettes[wifi.palette]);
      d.printf(width-2,palette_y,small,fg,display::TextAlign::TOP_RIGHT,"%d/%d",
        static_cast<int>(wifi.draft.value.size()),static_cast<int>(wifi.draft.limit));
      const auto &keys=wifi.glyphs();
      auto label=[&](int index)->std::string{
        if(index==3)return palettes[(wifi.palette+1)%4];
        if(index<4)return t[7+index];
        auto glyph=keys[index-4];return glyph==" "?"_":glyph;
      };
      // The bottom strip keeps neighboring characters visible while turning.
      int selected=wifi.header_selected()?0:wifi.key;
      if(selected>=4){
        int count=static_cast<int>(keys.size());
        for(int offset : {-2,-1,1,2}){
          int index=4+(selected-4+offset+count)%count;
          d.print(center+offset*step,height-1,body,fg,display::TextAlign::BOTTOM_CENTER,label(index).c_str());
        }
      }
      int selected_width=selected<4?std::min(100,width-4):22;
      if(!wifi.header_selected()){
        draw_selection(d,center-selected_width/2,key_top,selected_width,key_height,true,fg);
      }
      d.print(center,height-2,body,!wifi.header_selected()&&!menu.borders?bg:fg,
              display::TextAlign::BOTTOM_CENTER,label(selected).c_str());
      return;
    }
    int title=wifi.production_ui()?22:0;  // Real: "Wi-Fi" vs Demo: "Wi-Fi (demo)"
    switch(wifi.flow.stage) {
      case Stage::SCANNING:title=11;break;case Stage::CONNECTING:title=12;break;
      case Stage::SCAN_ERROR:title=18;break;
      case Stage::SUCCESS:title=wifi.production_ui()?23:13;break;
      case Stage::FAILURE:title=wifi.production_ui()?24:14;break;
      case Stage::RESULTS:if(!wifi.flow.results())title=17;break;
      default:if(wifi.flow.error==Error::SSID)title=15;
        else if(wifi.flow.error==Error::PASSWORD)title=16;break;
    }
    draw_form_header(d,small,fg,t[title],wifi.header_selected());
    int total=wifi.total();
    wifi.focus=std::min(wifi.focus,total-(wifi.production_ui()?0:1));
    const int row_height=std::max(17,body->get_height()+3);
    const int rows=std::max(1,(height-bar_height)/row_height);
    if(wifi.focus<total)wifi.top=nabla::scroll_anchor(wifi.focus,wifi.top,rows,total);
    for(int r=0;r<rows;r++){
      int index=wifi.top+r;if(index>=total)break;
      int y=bar_height+r*row_height;
      draw_selection(d,0,y,width,row_height,index==wifi.focus,fg);
      const int left=selection_inset(), available=width-left-4;
      d.start_clipping(left-1,y+1,width-2,y+row_height-2);
      auto label=wifi.row(index);int bx,by,bw,bh;
      d.get_text_bounds(0,0,label.c_str(),body,display::TextAlign::TOP_LEFT,&bx,&by,&bw,&bh);
      int offset=index==wifi.focus && bw>available?std::min(bw-available,std::max(0,int(((millis()-wifi_since)/100)%(bw-available+20))-10)):0;
      d.print(left-offset,y+2,body,index==wifi.focus && !menu.borders?bg:fg,label.c_str());d.end_clipping();
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
      draw_form_header(d,small,fg,forms.title().c_str());
    }
    int rows=menu.readable?1:3,rh=menu.readable?64-bar_height:17;
    forms.top=nabla::scroll_anchor(forms.focus,forms.top,rows,forms.total());
    for(int r=0;r<rows;r++){
      int index=forms.top+r;if(index>=forms.total())break;
      int y=bar_height+r*rh;bool selected=index==forms.focus;
      draw_selection(d,0,y,128,rh,selected,fg);
      const int left=selection_inset(), available=128-left-4;
      auto *font=menu.readable?large:body;const auto label=forms.row(index);
      int bx,by,bw,bh;
      d.get_text_bounds(0,0,label.c_str(),font,display::TextAlign::TOP_LEFT,&bx,&by,&bw,&bh);
      int offset=selected && bw>available?int(((millis()-form_since)/100)%(bw-available+20)):0;
      offset=std::clamp(offset-10,0,std::max(0,bw-available));
      d.start_clipping(left-1,y+1,124,y+rh-2);
      d.print(left-offset,y+(rh-bh)/2,font,selected && !menu.borders?bg:fg,label.c_str());d.end_clipping();
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
    draw_logo(d,fg,header_focus);
    d.start_clipping(16, 0, g.width-1, g.header-1);
    auto *title_font=small;
    if(menu.current==0){
      auto *regular=menu.font_family==0?title_ubuntu_preview:title_preview;
      if(regular)title_font=regular;
    }
    d.print(16, 0, title_font, fg, nodes[menu.current].title);
    d.end_clipping();
    const int n = children(menu.current);
    if(root_tiles && icons && menu.current==0 && n && single_icon_mode && menu.readable) {
      int current=menu.focus<n?menu.focus:std::min(menu.root_content_focus,n-1);
      auto draw_item=[&](int index,int offset){
        int node=child(0,index), y=g.header+offset, ch=g.content_height();
        bool selected=menu.focus<n;
        auto ink=selected&&!menu.borders?bg:fg;
        d.start_clipping(0,g.header,g.width-1,g.height-g.footer-1);
        if(selected&&!menu.borders)d.filled_rectangle(0,y,g.width,ch,fg);
        int bx,by,bw,bh;
        d.get_text_bounds(0,0,nodes[node].title,large,display::TextAlign::TOP_LEFT,&bx,&by,&bw,&bh);
        int icon_h=icons->get_height()-tiny_icon_reduce;
        int base_y=y+(ch-(icon_h+4+bh))/2-2;
        d.print(g.width/2,base_y+icon_h/2+2,icons,ink,display::TextAlign::CENTER,nodes[node].icon);
        d.print(g.width/2,base_y+icon_h+4,large,ink,display::TextAlign::TOP_CENTER,nodes[node].title);
        d.end_clipping();
      };
      float progress=std::min(1.0f,float(millis()-slide_since)/260.0f);
      if(slide_from>=0 && slide_to==current && progress<1.0f){
        float eased=progress*progress*(3.0f-2.0f*progress);
        int offset=int(lroundf(eased*g.content_height()));
        draw_item(slide_from,-slide_direction*offset);
        draw_item(slide_to,slide_direction*(g.content_height()-offset));
      }else {slide_from=-1;draw_item(current,0);}
    } else if(root_tiles && icons && menu.current==0 && n && (!single_icon_mode || menu.readable)) {
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
        // Tiny single-icon mode: use large font for label, centered at bottom.
        auto *label_font = (single_icon_mode && menu.readable) ? large : small;
        int bx,by,bw,bh;
        d.get_text_bounds(0,0,nodes[node].title,label_font,display::TextAlign::TOP_LEFT,&bx,&by,&bw,&bh);
        // For single_icon_mode, pack icon above label with less top padding.
        int icon_h = icons->get_height() - tiny_icon_reduce;
        // Position icon closer to top, label at bottom for better visual balance.
        int icon_y, label_y;
        if (single_icon_mode && menu.readable) {
          // Tiny icon mode: icon higher in content area, label at bottom.
          int total_h = icon_h + 4 + bh;  // icon + gap + label
          int base_y = y + (ch - total_h) / 2 - 2;  // shift UP for better visual balance
          icon_y = base_y + icon_h / 2;
          label_y = base_y + icon_h + 4;
        } else {
          // Standard mode: center icon+label vertically.
          int total_h = icon_h + 2 + bh;
          int base_y = y + (ch - total_h) / 2;
          icon_y = base_y + icon_h / 2;
          label_y = base_y + icon_h + 2;
        }
        d.print(x+cw/2,icon_y,icons,color,display::TextAlign::CENTER,nodes[node].icon);
        d.start_clipping(x+3,y,x+cw-4,y+ch-2);
        if(bw<=cw-6)d.print(x+cw/2,label_y,label_font,ink,display::TextAlign::TOP_CENTER,nodes[node].title);
        else {
          int offset=selected?std::min(bw-cw+6,std::max(0,int(((millis()-focus_since)/100)%(bw-cw+26))-10)):0;
          d.print(x+3-offset,label_y,label_font,ink,nodes[node].title);
        }
        d.end_clipping();
      }
    } else if (n) {
      // Use middle-scroll if enabled (selection stays in middle row when possible).
      int list_top = middle_scroll ? scroll_anchor_middle(header_focus ? (menu.current==0 ? menu.root_content_focus : std::min(menu.top+g.rows/2,n-1)) : menu.focus, g.rows, n) : menu.top;
      for (int r = 0; r < g.rows; ++r) {
        int index = list_top + r;
        if (index >= n) break;
        int y = g.header + r * g.row_height;
        bool selected = menu.focus == index;
        // Tiny profile: Normal mode uses play marker, Alto contraste uses inverted bar.
        bool use_play_marker = uses_play_marker();
        draw_selection(d,0,y,g.width,g.row_height,selected,fg);
        auto *font = menu.readable ? large : body;
        int row_node=child(menu.current,index);
        // With play marker, leave space for the marker on the left.
        int left = use_play_marker ? 10 : 4;
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
        // In play marker mode, text uses normal foreground (no inversion).
        d.print(left-offset, y+(g.row_height-bh)/2, font, (selected && !menu.borders && !use_play_marker) ? bg : fg, title);
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
      draw_selection(d,0,g.height-g.footer-g.header,g.width,g.header,true,fg);
      d.print(selection_inset(), g.height-g.footer-g.header, body, menu.borders?fg:bg, back_text);
    }
    if (g.footer) {
      d.print(2, g.height-2, small, fg, display::TextAlign::BOTTOM_LEFT, "NABLA");
      if (n) d.printf(g.width-2, g.height-2, small, fg, display::TextAlign::BOTTOM_RIGHT,
                     "%d/%d", menu.focus < n ? menu.focus+1 : std::min(menu.top+1,n), n);
    }
  }
  void touch(int x, int y) {
    if(in_forms()){
      if(y<bar_height){back();return;}
      int rh=menu.readable?64-bar_height:17,index=forms.top+(y-bar_height)/rh;
      if(y<64 && index<forms.total()){forms.focus=index;activate();}return;
    }
    if(in_wifi()) {
      if(y<bar_height) {back();return;}
      if(wifi.editing) {if(y>=44){if(x<44)move(-1);else if(x>84)move(1);else activate();}return;}
      int row=(y-bar_height)/17;
      if(y>=bar_height && row<3 && wifi.top+row<wifi.total()){wifi.focus=wifi.top+row;activate();}
      return;
    }
    if (y < bar_height) { if (x < 16) menu.touch_option(children(menu.current)); return; }
    if (!children(menu.current)) { if (y >= display_height-(app_footer ? bar_height : 0)-bar_height && y < display_height-(app_footer ? bar_height : 0)) menu.back(); return; }
    bool footer_visible=menu.readable ? (menu.current==0 || app_footer) : (menu.current==0 ? list_footer : app_footer);
    auto g = Geometry::compact(menu.readable, footer_visible,display_width,display_height,bar_height,menu.list_rows);
    if (y >= display_height-g.footer) return;
    if(root_tiles && icons && menu.current==0 && (!single_icon_mode || menu.readable)){
      int cells=menu.readable?1:4,cols=menu.readable?1:2;
      int first=menu.focus<children(0)?menu.focus/cells*cells:menu.top/cells*cells;
      int index=first+(y-bar_height)/(g.content_height()/(menu.readable?1:2))*cols+x/(g.width/cols);
      if(index<children(0))menu.touch_option(index);
      return;
    }
    int row = (y-bar_height)/g.row_height;
    int first=middle_scroll ? scroll_anchor_middle(menu.focus,g.rows,children(menu.current)) : menu.top;
    if (row < g.rows) {menu.touch_option(first + row);if(in_wifi())wifi.clear();if(in_forms())forms.begin();}
  }
};
inline CompactShell compact_shell;
}
