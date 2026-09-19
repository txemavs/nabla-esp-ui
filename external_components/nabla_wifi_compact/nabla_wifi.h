// Compact-device primary Wi-Fi adapter; supplies real scan/connect operations to the shared Wi-Fi flow.

#pragma once
#include "esphome/core/component.h"
#include "esphome/core/preferences.h"
#include "esphome/components/wifi/wifi_component.h"
#include "wifi_flow.h"
#include <array>
#include <vector>
#include <cstring>
namespace esphome::nabla_wifi {
class Adapter : public Component, public wifi::WiFiScanResultsListener, public nabla_forms::WifiBackend {
 public:
  struct Record { uint32_t magic=0; char ssid[33]{}; char password[65]{}; };
  static constexpr uint32_t MAGIC=0x4E574631;
  void add_fallback(const wifi::WiFiAP &ap) {if(fallbacks_.size()<8)fallbacks_.push_back(ap);}
  float get_setup_priority() const override {return setup_priority::WIFI-1;}
  void setup() override {
    wifi_=wifi::global_wifi_component;
    preference_=global_preferences->make_preference<Record>(0x4E414257,true);
    Record loaded{};
    if(preference_.load(&loaded) && loaded.magic==MAGIC && loaded.ssid[32]==0 &&
       loaded.password[64]==0 && valid_record(loaded)) saved_=loaded;
    wipe(loaded);
    wifi_->set_keep_scan_results(true);
    wifi_->add_scan_results_listener(this);
    nabla_forms::WifiFlow::backend=this;
    if(saved_.magic==MAGIC) restore();
  }
  void scan(nabla_forms::WifiFlow &flow,uint32_t now) override {
    if(active_) {flow.stage=nabla_forms::Stage::SCAN_ERROR;return;}
    active_=&flow;revision_=flow.revision;started_=now;connecting_=false;
    wifi_->start_scanning();
  }
  void connect(nabla_forms::WifiFlow &flow,uint32_t now) override {
    if(active_) {flow.stage=nabla_forms::Stage::FAILURE;return;}
    active_=&flow;revision_=flow.revision;started_=now;connecting_=true;stable_=0;
    wipe(candidate_);candidate_.magic=MAGIC;
    strncpy(candidate_.ssid,flow.ssid.c_str(),32);
    if(!flow.open)strncpy(candidate_.password,flow.password.c_str(),64);
    wifi_->disable();
    wifi_->set_sta(ap(candidate_));
    wifi_->enable();
  }
  void cancel(nabla_forms::WifiFlow &flow) override {
    if(active_!=&flow)return;
    bool reconnect=connecting_;
    active_=nullptr;connecting_=false;wipe(candidate_);
    if(reconnect)restore();
  }
  bool forget() override {
    if(active_)return false;
    Record empty{};
    if(!preference_.save(&empty) || !global_preferences->sync()){
      preference_.save(&saved_);global_preferences->sync();return false;
    }
    wipe(saved_);restore();return true;
  }
  void loop() override {
    if(!active_)return;
    if(active_->revision!=revision_){cancel(*active_);return;}
    uint32_t now=millis();
    if(connecting_){
      char ssid[wifi::SSID_BUFFER_SIZE];
      bool matched=wifi_->is_connected() && !strcmp(wifi_->wifi_ssid_to(ssid),candidate_.ssid);
      if(matched){
        if(!stable_)stable_=now;
        if(uint32_t(now-stable_)>=2000){
          auto *flow=active_;active_=nullptr;connecting_=false;
          bool ok=preference_.save(&candidate_) && global_preferences->sync();
          if(ok)saved_=candidate_;
          else {preference_.save(&saved_);global_preferences->sync();}
          wipe(candidate_);restore();
          flow->stage=ok?nabla_forms::Stage::SUCCESS:nabla_forms::Stage::FAILURE;
          return;
        }
      }else stable_=0;
      if(uint32_t(now-started_)>=30000){
        auto *flow=active_;active_=nullptr;connecting_=false;wipe(candidate_);
        restore();flow->stage=nabla_forms::Stage::FAILURE;
      }
    }else if(uint32_t(now-started_)>=15000){
      active_->stage=nabla_forms::Stage::SCAN_ERROR;active_=nullptr;
    }
  }
  void on_wifi_scan_results(const wifi::wifi_scan_vector_t<wifi::WiFiScanResult> &results) override {
    if(!active_ || connecting_ || active_->revision!=revision_)return;
    active_->found_count=0;
    // ESPHome owns scan storage. Copy at most eight unique SSID/security pairs.
    for(const auto &result:results)
      active_->publish_network(std::string(result.get_ssid().c_str()),!result.get_with_auth(),result.get_rssi());
    active_->stage=nabla_forms::Stage::RESULTS;active_=nullptr;
  }
 protected:
  static bool valid_record(const Record &r) {
    std::string name(r.ssid),password(r.password);
    return !name.empty() && nabla_forms::TextDraft::valid_utf8(name) &&
      (password.empty() || (password.size()>=8 && password.size()<=63 && nabla_forms::TextDraft::valid_utf8(password)));
  }
  static void wipe(Record &r){volatile unsigned char *p=reinterpret_cast<volatile unsigned char*>(&r);for(size_t i=0;i<sizeof(r);i++)p[i]=0;}
  wifi::WiFiAP ap(const Record &r) const {
    wifi::WiFiAP result;result.set_ssid(r.ssid);result.set_password(r.password);
    int priority=0;for(const auto &fallback:fallbacks_)priority=std::max(priority,int(fallback.get_priority()));
    result.set_priority(priority+10);return result;
  }
  void restore() {
    wifi_->disable();wifi_->clear_sta();
    wifi_->init_sta(fallbacks_.size()+(saved_.magic==MAGIC?1:0));
    if(saved_.magic==MAGIC)wifi_->add_sta(ap(saved_));
    for(const auto &fallback:fallbacks_)wifi_->add_sta(fallback);
    wifi_->enable();
  }
  wifi::WiFiComponent *wifi_{};
  std::vector<wifi::WiFiAP> fallbacks_;
  ESPPreferenceObject preference_;
  Record saved_{},candidate_{};
  nabla_forms::WifiFlow *active_{};
  uint32_t revision_=0,started_=0,stable_=0;
  bool connecting_=false;
};
}
