#pragma once
#include <array>
#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <functional>
namespace nabla_mqtt_lights {
struct State { bool on=false, available=false, dimmable=false; int brightness=0; };
class Model {
 public:
  std::array<State,16> states{};
  std::array<uint32_t,16> seen{}, pending{};
  std::array<bool,16> target_on{};
  std::array<int,16> target_level{};
  bool connected=false, dirty=false;
  std::string log="Waiting for MQTT\n";
  void note(const std::string &line) { log=line+"\n"+log; if(log.size()>900)log.resize(900); dirty=true; }
  bool valid(int i) const {return i>=0 && i<16;}
  bool available(int i,uint32_t now) const {return valid(i)&&connected&&seen[i]&&now-seen[i]<90000&&states[i].available;}
  bool ready(int i,uint32_t now) const {return available(i,now)&&!pending[i];}
  void connect(bool value) {
    connected=value;
    for(int i=0;i<16;i++){states[i].available=false;seen[i]=0;pending[i]=0;}
    note(value?"MQTT connected; waiting for state":"MQTT disconnected");
  }
  void update(int i,State value,uint32_t now) {
    if(!valid(i))return;
    value.brightness=std::clamp(value.brightness,0,100);
    const auto &old=states[i];
    if(!seen[i] || old.on!=value.on || old.available!=value.available ||
       old.dimmable!=value.dimmable || old.brightness!=value.brightness)dirty=true;
    states[i]=value;seen[i]=now?now:1;
    if(pending[i] && value.available && value.on==target_on[i] &&
       (!value.on || !value.dimmable || std::abs(value.brightness-target_level[i])<=1)){
      pending[i]=0;note("RX confirmed "+std::to_string(i+1));
    }
  }
  void sent(int i,bool on,int level,uint32_t now){
    if(!valid(i))return;target_on[i]=on;target_level[i]=level;pending[i]=now?now:1;
    note("TX "+std::to_string(i+1)+(on?" ON":" OFF"));
  }
  void tick(uint32_t now) {
    for(int i=0;i<16;i++){
      if(seen[i]&&now-seen[i]>=90000&&states[i].available){states[i].available=false;dirty=true;}
      if(pending[i]&&now-pending[i]>=15000){pending[i]=0;note("No confirmation "+std::to_string(i+1));}
    }
  }
};
inline Model model;
class CompactDraft {
 public:
  int slot=-1,level=0,focus=0;
  std::string title;
  std::function<void(int,bool,int)> send;
  const char *apply="Apply",*cancel="Cancel",*unavailable="Unavailable",*adjust="Brightness";
  bool active()const{return slot>=0;}
  void open(int index,const char *name,uint32_t now){
    if(!model.ready(index,now))return;
    auto s=model.states[index];
    if(!s.dimmable){if(send)send(index,!s.on,s.brightness);return;}
    slot=index;title=std::string(name).substr(0,64);level=s.on?s.brightness:0;focus=0;
  }
  bool move(int delta){
    if(!active())return false;
    if(focus==0)level=std::clamp(level+delta,0,100);
    else focus=(focus+delta%3+3)%3;
    return true;
  }
  bool back(){if(!active())return false;slot=-1;return true;}
  bool activate(uint32_t now){
    if(!active())return false;
    if(focus==0)focus=1;
    else if(focus==2)slot=-1;
    else if(model.ready(slot,now)){int target=slot;slot=-1;if(send)send(target,level>0,level);}
    return true;
  }
};
}
