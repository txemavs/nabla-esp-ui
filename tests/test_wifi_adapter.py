# Wi-Fi adapter regression checks; verifies state transitions and recovery using controlled fixtures.

from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[1]
class WifiAdapterTests(unittest.TestCase):
    def test_trial_cancel_timeout_persistence_and_fallbacks(self):
        stubs = r'''
#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
namespace esphome {
inline uint32_t clock_now=100;
inline uint32_t millis(){return clock_now;}
namespace setup_priority {constexpr float WIFI=250;}
struct Component {virtual ~Component()=default;virtual void setup(){};virtual void loop(){};virtual float get_setup_priority() const{return 0;}};
inline std::vector<uint8_t> stored;
inline bool fail_save=false,fail_sync=false;
struct ESPPreferenceObject {
 template<class T> bool load(T *v){if(stored.size()!=sizeof(T))return false;memcpy(v,stored.data(),sizeof(T));return true;}
 template<class T> bool save(const T *v){if(fail_save)return false;stored.assign(reinterpret_cast<const uint8_t*>(v),reinterpret_cast<const uint8_t*>(v)+sizeof(T));return true;}
};
struct Preferences {
 template<class T> ESPPreferenceObject make_preference(uint32_t,bool){return {};}
 bool sync(){return !fail_sync;}
};
inline Preferences prefs;inline Preferences *global_preferences=&prefs;
namespace wifi {
constexpr int SSID_BUFFER_SIZE=33;
struct WiFiAP{
 std::string ssid,password;int priority=0;
 void set_ssid(const std::string &v){ssid=v;}void set_password(const std::string &v){password=v;}
 void set_priority(int v){priority=v;}int get_priority()const{return priority;}
};
struct WiFiScanResult{
 std::string name;bool auth;int signal;
 std::string get_ssid()const{return name;}bool get_with_auth()const{return auth;}int get_rssi()const{return signal;}
};
template<class T> using wifi_scan_vector_t=std::vector<T>;
struct WiFiScanResultsListener{virtual void on_wifi_scan_results(const wifi_scan_vector_t<WiFiScanResult>&)=0;};
struct WiFiComponent{
 std::vector<WiFiAP> aps;bool connected=false;std::string ssid;int scans=0;
 void set_keep_scan_results(bool){}void add_scan_results_listener(WiFiScanResultsListener*){}
 void disable(){connected=false;}void enable(){}void clear_sta(){aps.clear();}
 void init_sta(size_t){}void add_sta(const WiFiAP &a){aps.push_back(a);}
 void set_sta(const WiFiAP &a){aps={a};}void start_scanning(){scans++;}
 bool is_connected()const{return connected;}
 const char *wifi_ssid_to(char *buf){strncpy(buf,ssid.c_str(),32);buf[32]=0;return buf;}
};
inline WiFiComponent radio;inline WiFiComponent *global_wifi_component=&radio;
}}
'''
        program = r'''
#include "external_components/nabla_wifi/nabla_wifi.h"
#include <cassert>
using namespace esphome;
using nabla_forms::WifiFlow;using nabla_forms::Stage;
using esphome::nabla_wifi::Adapter;
void fallbacks(Adapter &a){
 for(int i=0;i<3;i++){wifi::WiFiAP ap;ap.set_ssid("fallback"+std::to_string(i));ap.set_password("original-password");ap.set_priority(30-i*10);a.add_fallback(ap);}
}
void trial(WifiFlow &f){f.ssid="primary";f.password="valid-password";assert(f.connect(clock_now));assert(f.password.empty());}
int main(){
 Adapter a;fallbacks(a);a.setup();WifiFlow f;
 trial(f);assert(wifi::radio.aps.size()==1);assert(!f.connect(clock_now));
 f.cancel();assert(wifi::radio.aps.size()==3);assert(stored.empty());
 trial(f);clock_now+=30001;a.loop();assert(f.stage==Stage::FAILURE);assert(wifi::radio.aps.size()==3);assert(stored.empty());
 f.cancel();trial(f);wifi::radio.connected=true;wifi::radio.ssid="wrong";clock_now+=30001;a.loop();assert(f.stage==Stage::FAILURE);
 f.cancel();trial(f);wifi::radio.connected=true;wifi::radio.ssid="primary";a.loop();clock_now+=2001;a.loop();
 assert(f.stage==Stage::SUCCESS);assert(wifi::radio.aps.size()==4);assert(wifi::radio.aps[0].priority==40);
 for(int i=0;i<3;i++)assert(wifi::radio.aps[i+1].ssid=="fallback"+std::to_string(i));
 auto saved=stored;
 Adapter reboot;fallbacks(reboot);reboot.setup();assert(wifi::radio.aps.size()==4);
 f.cancel();trial(f);f.cancel();assert(stored==saved);assert(wifi::radio.aps.size()==4);
 assert(f.scan(clock_now));reboot.on_wifi_scan_results({{"same",true,-80},{"same",true,-30},{"same",false,-50}});
 assert(f.results()==2);assert(f.result(0).rssi==-30);assert(f.choose(1));assert(f.open);assert(f.ssid=="same");
 f.cancel();assert(f.scan(clock_now));f.cancel();reboot.on_wifi_scan_results({{"late",true,-10}});assert(f.stage==Stage::EDIT);
 assert(f.scan(clock_now));clock_now+=15001;reboot.loop();assert(f.stage==Stage::SCAN_ERROR);
 f.cancel();trial(f);wifi::radio.connected=true;wifi::radio.ssid="primary";reboot.loop();fail_save=true;clock_now+=2001;reboot.loop();
 assert(f.stage==Stage::FAILURE);assert(stored==saved);fail_save=false;
 f.cancel();f.ssid="changed";f.password="changed-pass";assert(f.connect(clock_now));
 wifi::radio.connected=true;wifi::radio.ssid="changed";reboot.loop();
 fail_sync=true;clock_now+=2001;reboot.loop();assert(f.stage==Stage::FAILURE);assert(stored==saved);fail_sync=false;
 assert(reboot.forget());assert(wifi::radio.aps.size()==3);
 Adapter cleared;fallbacks(cleared);cleared.setup();assert(wifi::radio.aps.size()==3);
 clock_now=0xfffffff0u;f.cancel();trial(f);clock_now+=30001;cleared.loop();assert(f.stage==Stage::FAILURE);
 WifiFlow::backend=nullptr;
}
'''
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)
            (root/"stubs.h").write_text(stubs)
            for rel in ("esphome/core/component.h","esphome/core/preferences.h","esphome/components/wifi/wifi_component.h"):
                p=root/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_text('#include "stubs.h"\n')
            (root/"test.cpp").write_text(program)
            subprocess.run(["g++","-std=c++17","-fsanitize=address,undefined","-fno-omit-frame-pointer","-I",str(root),"-I",str(ROOT),"-I",str(ROOT/"components/forms"),str(root/"test.cpp"),"-o",str(root/"test")],check=True)
            subprocess.run([str(root/"test")],check=True)
