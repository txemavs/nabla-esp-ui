// HFP service-level bring-up; no microphone capture or Assist bridge yet.
#pragma once
#include "esphome/core/component.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_hf_ag_api.h"
#include "esp_hf_ag_legacy_api.h"
#include "esp_timer.h"
#include <atomic>
#include <cstring>
#include <array>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
namespace esphome::nabla_bt_audio {
class Gateway : public Component {
 public:
  void set_peer(uint64_t peer) { for(int i=5;i>=0;--i){peer_[i]=peer&255;peer>>=8;} }
  void setup() override;
  void loop() override;
  void scan();
  void test_audio();
  void select(size_t slot);
  void confirm();
  bool available(size_t slot) const { return slot<count_ && !scanning_ && !waiting_ && !connected(); }
  bool needs_confirmation() const { return confirm_; }
  size_t candidate_count() const { return count_; }
  std::string candidate(size_t slot) const { return slot<count_ ? candidates_[slot].name : ""; }
  void connect();
  void disconnect();
  bool connected() const { return state_==ESP_HF_CONNECTION_STATE_SLC_CONNECTED; }
  const char *status() const { return status_; }
 protected:
  static Gateway *instance_;
  static uint32_t outgoing(uint8_t *data,uint32_t length);
  static void incoming(const uint8_t *,uint32_t) {}
  static void tick(void *) { esp_hf_ag_outgoing_data_ready(); }
  esp_timer_handle_t timer_{nullptr};
  std::atomic<int> audio_event_{-1}, remaining_{0}, rate_{8000};
  uint32_t tone_started_{0};
  bool tone_requested_{false}, tone_running_{false};
  uint32_t phase_{0};
  static void callback(esp_hf_cb_event_t event, esp_hf_cb_param_t *param);
  static void gap(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);
  struct Discovery { uint8_t address[6]{}; char name[64]{}; bool done{false}; int kind{0}; uint32_t value{0}; };
  std::array<Discovery,8> candidates_{};
  QueueHandle_t discoveries_{nullptr};
  size_t count_{0};
  bool scanning_{false}, confirm_{false};
  std::string pairing_status_;
  uint32_t scan_started_{0};
  uint8_t peer_[6]{};
  std::atomic<int> pending_{-1};
  int state_{ESP_HF_CONNECTION_STATE_DISCONNECTED};
  bool ready_{false}, waiting_{false};
  uint32_t started_{0};
  const char *status_{"Starting"};
};
} // namespace
