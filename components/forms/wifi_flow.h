#pragma once
#include <cstdint>
#include <string>
#include <cstring>
namespace nabla_forms {
enum class Stage { EDIT, SCANNING, RESULTS, CONNECTING, SUCCESS, FAILURE };
enum class Error { NONE, SSID, PASSWORD };
struct Network { const char *ssid; bool open; bool fails; };
inline constexpr Network networks[] = {
  {"Nabla Demo", false, false}, {"Invitados Demo", true, false},
  {"Demo sin servicio", false, true}, {"Café Demo", false, false}
};
inline constexpr int network_count = sizeof(networks)/sizeof(networks[0]);
struct TextDraft {
  std::string value;
  size_t limit = 32;
  bool append(const std::string &glyph) {
    if (value.size()+glyph.size()>limit) return false;
    value += glyph; return true;
  }
  void erase() {
    if (value.empty()) return;
    size_t n=value.size()-1;
    while (n>0 && (static_cast<unsigned char>(value[n]) & 0xC0)==0x80) --n;
    value.erase(n);
  }
  void clear() { for (char &c:value) c=0; value.clear(); }
};
struct WifiFlow {
  std::string ssid, password;
  bool open = false, empty_scan = false;
  int selected_network = -1;
  Stage stage = Stage::EDIT;
  Error error = Error::NONE;
  uint32_t started = 0, revision = 0;
  void wipe_password() { for(char &c:password)c=0; password.clear(); }
  void clear() { wipe_password(); ssid.clear(); open=false; selected_network=-1; cancel(); }
  void cancel() { stage=Stage::EDIT; error=Error::NONE; ++revision; }
  void scan(uint32_t now, bool empty=false) {
    stage=Stage::SCANNING; started=now; empty_scan=empty; error=Error::NONE; ++revision;
  }
  int results() const { return empty_scan ? 0 : network_count; }
  bool choose(int index) {
    if(stage!=Stage::RESULTS || index<0 || index>=results()) return false;
    selected_network=index; ssid=networks[index].ssid; open=networks[index].open;
    wipe_password(); stage=Stage::EDIT; error=Error::NONE; return true;
  }
  bool connect(uint32_t now) {
    if(stage==Stage::CONNECTING || stage==Stage::SCANNING) return false;
    error = ssid.empty() || ssid.size()>32 ? Error::SSID :
      !open && (password.size()<8 || password.size()>63) ? Error::PASSWORD : Error::NONE;
    if(error!=Error::NONE) return false;
    started=now; stage=Stage::CONNECTING; ++revision; wipe_password(); return true;
  }
  void tick(uint32_t now) {
    if(stage==Stage::SCANNING && uint32_t(now-started)>=700) stage=Stage::RESULTS;
    if(stage==Stage::CONNECTING && uint32_t(now-started)>=1000)
      stage=selected_network>=0 && selected_network<network_count && ssid==networks[selected_network].ssid &&
        networks[selected_network].fails ? Stage::FAILURE : Stage::SUCCESS;
  }
};
}
