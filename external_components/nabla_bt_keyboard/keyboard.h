// Bounded classic HID host. Bluetooth callbacks enqueue; UI callbacks run in loop().
#pragma once
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/preferences.h"
#include "esp_gap_bt_api.h"
#include "esp_hidh_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "keymap.h"
#include "report.h"
#include <array>
#include <atomic>
#include <string>
namespace esphome::nabla_bt_keyboard {
class Keyboard : public Component {
 public:
  void setup() override;
  void loop() override;
  void scan();
  void connect_slot(int slot);
  void confirm();
  void disconnect();
  void forget();
  bool available(int slot) const { return slot>=0 && slot<count_ && !connected_ && !connecting_; }
  std::string candidate(int slot) const { return slot>=0 && slot<count_ ? candidates_[slot].name : "--"; }
  std::string status() const { return status_; }
  std::string pairing() const { return pairing_; }
  void add_key_callback(std::function<void(uint8_t,uint8_t)> cb) { keys_.add(std::move(cb)); }
 protected:
  struct Candidate { uint8_t address[6]{}; char name[40]{}; };
  struct Event { int kind{}; int value{}; uint8_t address[6]{}; uint8_t report[33]{}; ReportLayout layout{}; uint16_t length{}; int mode{}; char name[40]{}; };
  static Keyboard *instance_;
  static void gap(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *p);
  static void hid(esp_hidh_cb_event_t event, esp_hidh_cb_param_t *p);
  static void enqueue(const Event &e);
  QueueHandle_t queue_{nullptr};
  Candidate candidates_[4]{};
  int count_{0};
  int pending_slot_{-1};
  ReportLayout report_layout_{};
  uint8_t selected_[6]{}, previous_[6]{}, handle_{0};
  bool ready_{false}, scanning_{false}, connecting_{false}, connected_{false}, boot_{false}, confirmation_{false};
  uint32_t started_{0};
  uint32_t reconnect_at_{5000};
  uint8_t reconnect_attempts_{0};
  bool auto_reconnect_{true};
  std::atomic<bool> overflow_{false};
  ESPPreferenceObject preference_;
  uint8_t saved_[6]{};
  std::string status_{"Starting"}, pairing_{"--"};
  CallbackManager<void(uint8_t,uint8_t)> keys_;
};
class KeyTrigger : public Trigger<uint8_t,uint8_t> {
 public:
  explicit KeyTrigger(Keyboard *parent) {
    parent->add_key_callback([this](uint8_t key,uint8_t modifiers){ this->trigger(key,modifiers); });
  }
};
}  // namespace
