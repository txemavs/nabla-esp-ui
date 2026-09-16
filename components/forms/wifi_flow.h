#pragma once
#include <cstdint>
#include <string>
#include <cstring>
#include <array>
namespace nabla_forms {
enum class Stage { EDIT, SCANNING, RESULTS, CONNECTING, SUCCESS, FAILURE, SCAN_ERROR };
enum class Error { NONE, SSID, PASSWORD };
enum class ScanMode { NORMAL, EMPTY, ERROR };
struct Network { const char *ssid; bool open; bool fails; int rssi=-60; };
inline constexpr Network networks[] = {
  {"Nabla Demo", false, false, -60}, {"Invitados Demo", true, false, -50},
  {"Demo sin servicio", false, true, -70}, {"Café Demo", false, false, -65},
  {"Nabla-12345678901234567890123456",false,false,-80},
  {"Nabla Demo",true,false,-75},
  {"Nabla Demo",false,false,-35}
};
inline constexpr int network_count = sizeof(networks)/sizeof(networks[0]);
struct TextDraft {
  std::string value;
  size_t limit = 32;
  static bool valid_utf8(const std::string &s) {
    for(size_t i=0;i<s.size();){
      unsigned char c=s[i++];uint32_t code=c;int extra=0;
      if(c==0)return false;
      if(c<0x80)continue;
      if(c>=0xC2 && c<=0xDF){code=c&31;extra=1;}
      else if(c>=0xE0 && c<=0xEF){code=c&15;extra=2;}
      else if(c>=0xF0 && c<=0xF4){code=c&7;extra=3;}
      else return false;
      if(i+extra>s.size())return false;
      for(int j=0;j<extra;j++){unsigned char next=s[i++];if((next&0xC0)!=0x80)return false;code=(code<<6)|(next&63);}
      if((extra==2 && code<0x800)||(extra==3 && code<0x10000)||
         (code>=0xD800 && code<=0xDFFF)||code>0x10FFFF)return false;
    }
    return true;
  }
  bool append(const std::string &glyph) {
    if(value.size()+glyph.size()>limit || !valid_utf8(glyph))return false;
    value+=glyph;return true;
  }
  void erase() {
    if(value.empty())return;
    size_t n=value.size()-1;
    while(n>0 && (static_cast<unsigned char>(value[n])&0xC0)==0x80)--n;
    value.erase(n);
  }
  void clear(){for(char &c:value)c=0;value.clear();}
};
struct WifiFlow {
  std::string ssid,password;
  bool open=false;
  int selected_network=-1;
  Stage stage=Stage::EDIT;
  Error error=Error::NONE;
  ScanMode scan_mode=ScanMode::NORMAL;
  uint32_t started=0,revision=0,accepted=0;
  std::array<int,8> found{};
  int found_count=0;
  bool pending_failure=false;
  void wipe_password(){for(char &c:password)c=0;password.clear();}
  void clear(){wipe_password();ssid.clear();open=false;selected_network=-1;found_count=0;cancel();}
  void cancel(){stage=Stage::EDIT;error=Error::NONE;pending_failure=false;++revision;}
  bool scan(uint32_t now,ScanMode mode){
    if(stage==Stage::SCANNING || stage==Stage::CONNECTING)return false;
    stage=Stage::SCANNING;started=now;scan_mode=mode;found_count=0;
    error=Error::NONE;++revision;return true;
  }
  bool scan(uint32_t now,bool empty=false){return scan(now,empty?ScanMode::EMPTY:ScanMode::NORMAL);}
  int results()const{return stage==Stage::RESULTS?found_count:0;}
  const Network &result(int index)const{return networks[found[index]];}
  std::string result_label(int index)const{
    const auto &n=result(index);
    return std::string(n.open?"[O] ":"[P] ")+n.ssid;
  }
  bool choose(int index){
    if(index<0 || index>=results())return false;
    selected_network=found[index];ssid=networks[selected_network].ssid;open=networks[selected_network].open;
    wipe_password();stage=Stage::EDIT;error=Error::NONE;return true;
  }
  bool connect(uint32_t now){
    if(stage!=Stage::EDIT && stage!=Stage::FAILURE)return false;
    error=ssid.empty() || ssid.size()>32 || !TextDraft::valid_utf8(ssid)?Error::SSID:
      !open && (password.size()<8 || password.size()>63 || !TextDraft::valid_utf8(password))?Error::PASSWORD:Error::NONE;
    if(error!=Error::NONE)return false;
    pending_failure=selected_network>=0 && selected_network<network_count &&
      ssid==networks[selected_network].ssid && open==networks[selected_network].open && networks[selected_network].fails;
    started=now;stage=Stage::CONNECTING;++revision;++accepted;wipe_password();return true;
  }
  void tick(uint32_t now){
    if(stage==Stage::SCANNING && uint32_t(now-started)>=700){
      if(scan_mode==ScanMode::ERROR){stage=Stage::SCAN_ERROR;return;}
      stage=Stage::RESULTS;
      if(scan_mode==ScanMode::EMPTY)return;
      for(int i=0;i<network_count;i++){
        int duplicate=-1;
        for(int j=0;j<found_count;j++)
          if(networks[found[j]].open==networks[i].open && !strcmp(networks[found[j]].ssid,networks[i].ssid))duplicate=j;
        if(duplicate>=0){
          if(networks[i].rssi>networks[found[duplicate]].rssi)found[duplicate]=i;
        }else if(found_count<int(found.size()))found[found_count++]=i;
      }
    }
    if(stage==Stage::CONNECTING && uint32_t(now-started)>=1000)
      stage=pending_failure?Stage::FAILURE:Stage::SUCCESS;
  }
};
}
