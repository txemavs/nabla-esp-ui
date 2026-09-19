// Compact light editor rendering and input; reuses the shared MQTT state and draft-confirmation rules.

#pragma once
#include "mqtt_model.h"
#include "esphome/components/display/display.h"
#include "esphome/components/font/font.h"
#include <functional>
namespace nabla_mqtt_lights {
class CompactEditor : public CompactDraft {
 public:
  bool render(esphome::display::Display &d,esphome::font::Font *small,esphome::font::Font *body,bool dark,uint32_t now){
    if(!active())return false;
    using namespace esphome;
    auto fg=dark?Color::WHITE:Color::BLACK,bg=dark?Color::BLACK:Color::WHITE;
    int w=d.get_width(),h=d.get_height();
    d.fill(bg);d.start_clipping(2,0,w-3,15);d.print(2,0,small,fg,title.c_str());d.end_clipping();
    if(!model.ready(slot,now))d.print(w/2,24,small,fg,display::TextAlign::TOP_CENTER,unavailable);
    else d.print(w/2,24,small,fg,display::TextAlign::TOP_CENTER,adjust);
    d.printf(w/2,40,body,fg,display::TextAlign::TOP_CENTER,"%d%%",level);
    d.rectangle(8,h/2+8,w-16,8,fg);d.filled_rectangle(9,h/2+9,(w-18)*level/100,6,fg);
    int y=h-20;
    for(int i=1;i<=2;i++){
      int x=(i-1)*w/2;bool selected=focus==i;
      if(selected)d.filled_rectangle(x,y,w/2,20,fg);
      d.print(x+w/4,y+2,small,selected?bg:fg,display::TextAlign::TOP_CENTER,i==1?apply:cancel);
    }
    return true;
  }
};
inline CompactEditor compact_editor;
}
