// Bounded, self-releasing tap state shared by HTTP admission and LVGL input.
#pragma once
#include <cstdint>
namespace esphome::nabla_display_mirror {
inline bool parse_coordinate(const char *text,int limit,int &value){
  if(!text || !*text)return false;
  value=0;
  for(const char *p=text;*p;++p){if(*p<'0'||*p>'9')return false;value=value*10+(*p-'0');if(value>=limit)return false;}
  return true;
}
class RemoteTap {
 public:
  int x=0,y=0;
  bool enqueue(int px,int py,uint32_t now){if(pending_||pressed_)return false;x=px;y=py;started_=now;pending_=true;return true;}
  bool read(uint32_t now){
    if(pending_ && uint32_t(now-started_)>500){cancel();return false;}
    if(pending_){pending_=false;pressed_=true;started_=now;}
    else if(pressed_ && uint32_t(now-started_)>=80)pressed_=false;
    return pressed_;
  }
  void cancel(){pending_=pressed_=false;}
 private:
  bool pending_=false,pressed_=false;uint32_t started_=0;
};
}
