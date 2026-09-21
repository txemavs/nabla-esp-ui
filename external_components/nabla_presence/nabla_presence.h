// Bounded retained address announcements; no UI work or control traffic over MQTT.
#pragma once
#include <atomic>
#include "esphome/core/application.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/alloc_helpers.h"
#include "esphome/components/json/json_util.h"
#include "esphome/components/mqtt/mqtt_client.h"
#include "esphome/components/wifi/wifi_component.h"
#include "esphome/components/web_server_base/web_server_base.h"

namespace esphome::nabla_presence {
class Presence : public PollingComponent, public AsyncWebHandler {
 public:
  Presence() : PollingComponent(5000) {}
  void set_prefix(const std::string &prefix) { prefix_ = prefix; }
  float get_setup_priority() const override { return setup_priority::WIFI + 2; }
  void setup() override {
    identity_ = "esp32-" + get_mac_address();
    char boot[17];
    snprintf(boot, sizeof(boot), "%08x%08x", (unsigned) random_uint32(), (unsigned) random_uint32());
    boot_ = boot;
    topic_ = prefix_ + "/" + identity_ + "/announce";
    auto *base = web_server_base::global_web_server_base;
    base->init();
    base->add_handler(this);
    mqtt::global_mqtt_client->set_on_connect([this](bool) { reconnect_.store(true); });
  }
  bool canHandle(AsyncWebServerRequest *request) const override {
    char url[AsyncWebServerRequest::URL_BUF_SIZE];
    return request->url_to(url) == "/nabla/identity";
  }
  void handleRequest(AsyncWebServerRequest *request) override {
    if (request->method() != HTTP_GET) { request->send(405); return; }
    // Immutable after setup; safe to read from the HTTP callback.
    auto payload = json::build_json([this](JsonObject root) {
      root["v"] = 1; root["device_id"] = identity_; root["boot_id"] = boot_;
    });
    auto *response = request->beginResponse(200, "application/json", payload.c_str());
    response->addHeader("Cache-Control", "no-store");
    request->send(response);
  }
  void update() override {
    auto *wifi = wifi::global_wifi_component;
    auto *mqtt = mqtt::global_mqtt_client;
    if (!wifi->is_connected() || !mqtt->is_connected()) { connected_ = false; return; }
    char ip_buffer[network::IP_ADDRESS_BUFFER_SIZE];
    const std::string ip = wifi->get_ip_addresses()[0].str_to(ip_buffer);
    if (reconnect_.exchange(false)) connected_ = false;
    if (ip.empty() || ip == "0.0.0.0") return;
    const uint32_t now = millis();
    if (connected_ && ip == last_ip_ && now - last_publish_ < 60000) return;
    const bool sent = mqtt->publish_json(topic_, [this, &ip](JsonObject root) {
      root["v"] = 1; root["device_id"] = identity_; root["boot_id"] = boot_;
      root["name"] = App.get_name(); root["host"] = ip;
    }, 1, true);
    if (sent) { connected_ = true; last_ip_ = ip; last_publish_ = now; }
  }
 protected:
  std::string prefix_, identity_, boot_, topic_, last_ip_;
  uint32_t last_publish_{0};
  bool connected_{false};
  std::atomic<bool> reconnect_{true};
};
}  // namespace esphome::nabla_presence
