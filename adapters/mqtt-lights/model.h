#pragma once
#include <array>
#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <string>
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
    states[i]=value;seen[i]=now?now:1;dirty=true;
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
}
