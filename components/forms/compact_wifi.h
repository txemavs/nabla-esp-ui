// Compact Wi-Fi form input and rendering; adapts the shared form flow to small displays.

#pragma once
#include "wifi_flow.h"
#include "focus.h"
#include <vector>
#include <array>
namespace nabla_forms {
struct CompactWifi {
  WifiFlow flow;
  TextDraft draft;
  int focus=0, top=0, key=0, palette=0;
  bool editing=false, secret=false, clean_ui=false;
  bool production_ui() const {return clean_ui || flow.real();}
  bool header_selected() const {return production_ui() && (editing?key:focus)==total();}
  const char *const *text=nullptr;
  const std::vector<std::string> &glyphs() const {
    static const auto palettes=[](){
    static const char *sets[]={"abcdefghijklmnopqrstuvwxyzáéíóúñü", "ABCDEFGHIJKLMNOPQRSTUVWXYZÁÉÍÓÚÑÜ",
      "0123456789", " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"};
      std::array<std::vector<std::string>,4> out;
      for(int i=0;i<4;i++){
        const char *s=sets[i];
        while(*s){size_t n=static_cast<unsigned char>(*s)>=0xC0?2:1;out[i].emplace_back(s,n);s+=n;}
      }
      return out;
    }();
    return palettes[palette];
  }
  void clear() {flow.clear();draft.clear();focus=top=key=palette=0;editing=false;}
  int total() const {
    if(editing) return 4+glyphs().size();
    if(flow.stage==Stage::RESULTS) return flow.results()+1;
    if(flow.stage!=Stage::EDIT) return 1;
    return production_ui() ? 5 : 8;  // Real: SSID/Pass/Open/Scan/Connect/Back; Mock adds demo items
  }
  void move(int delta) {
    if(editing) {key=nabla::wrap_focus(key,delta,total()+(production_ui()?1:0));return;}
    focus=nabla::wrap_focus(focus,delta,total()+(production_ui()?1:0));
    if(focus<total())top=nabla::scroll_anchor(focus,top,3,total());
  }
  void edit(bool password) {
    secret=password;draft.value=password?flow.password:flow.ssid;draft.limit=password?63:32;
    key=production_ui()?4:0;editing=true;
  }
  bool back() {
    if(editing) {draft.clear();editing=false;return false;}
    if(flow.stage!=Stage::EDIT) {flow.cancel();focus=top=0;return false;}
    clear();return true;
  }
  bool activate(uint32_t now) {
    if(header_selected())return back();
    if(editing) {
      if(key==0) {
        if(secret) flow.password=draft.value; else {flow.ssid=draft.value;flow.selected_network=-1;}
        draft.clear();editing=false;flow.error=Error::NONE;
      } else if(key==1) draft.erase();
      else if(key==2) {draft.clear();editing=false;}
      else if(key==3) {palette=(palette+1)%4;key=4;}
      else {const auto &g=glyphs();if(key-4<(int)g.size())draft.append(g[key-4]);}
      return false;
    }
    if(flow.stage==Stage::RESULTS) {
      if(focus<flow.results()) {
        flow.choose(focus);
        if(production_ui() && !flow.open)edit(true);
      } else flow.cancel();
      focus=top=0;return false;
    }
    if(flow.stage!=Stage::EDIT) {flow.cancel();focus=top=0;return false;}
    if(production_ui()) {
      switch(focus) {
        case 0: edit(false);break;
        case 1: edit(true);break;
        case 2: flow.open=!flow.open;flow.wipe_password();break;
        case 3: flow.scan(now);focus=top=0;break;
        case 4:
          if(!flow.connect(now) && flow.error!=Error::NONE){
            focus=flow.error==Error::SSID?0:1;top=0;
          }
          break;
      }
    } else {
      switch(focus) {
        case 0: edit(false);break;
        case 1: edit(true);break;
        case 2: flow.open=!flow.open;flow.wipe_password();break;
        case 3: flow.scan(now);focus=top=0;break;
        case 4: flow.scan(now,true);focus=top=0;break;
        case 5: flow.scan(now,ScanMode::ERROR);focus=top=0;break;
        case 6:
          if(!flow.connect(now) && flow.error!=Error::NONE){
            focus=flow.error==Error::SSID?0:1;top=0;
          }
          break;
        case 7: return back();
      }
    }
    return false;
  }
  std::string row(int index) const {
    if(flow.stage==Stage::RESULTS)
      return index<flow.results()?flow.result_label(index):text[6];
    if(flow.stage!=Stage::EDIT) return text[6];
    if(production_ui()) {
      switch(index) {
        case 0:return std::string("SSID: ")+(flow.ssid.empty()?"...":flow.ssid);
        case 1:return std::string(text[1])+": "+(flow.password.empty()?"...":"********");
        case 2:return std::string(flow.open?"[x] ":"[ ] ")+text[2];
        case 3:return text[3];  // Scan
        case 4:return text[25]; // Connect
        default:return text[6]; // Back
      }
    }
    switch(index) {
      case 0:return std::string("SSID: ")+(flow.ssid.empty()?"...":flow.ssid);
      case 1:return std::string(text[1])+": "+(flow.password.empty()?"...":"********");
      case 2:return std::string(flow.open?"[x] ":"[ ] ")+text[2];
      case 3:return text[3];
      case 4:return text[4];
      case 5:return text[19];
      case 6:return text[5];
      default:return text[6];
    }
  }
};
}
