#pragma once
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/runtime_image/runtime_image.h"
#include "esp_http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <vector>
#include <string>
namespace esphome::nabla_camera_fetch {
class Fetcher : public Component {
 public:
  void add_image(runtime_image::RuntimeImage *image) { images_.push_back(image); }
  void setup() override;
  void loop() override;
  void select(int slot) { if(selected_!=slot){selected_=slot; ++generation_;} }
  bool request(int slot,const std::string &url);
  template<typename F> void add_callback(F &&cb) { callbacks_.add(std::forward<F>(cb)); }
 protected:
  static constexpr size_t LIMIT=128*1024;
  struct Job { int slot; uint32_t generation; char url[1024]; };
  struct Result { int slot; uint32_t generation; uint8_t *data; size_t size; bool ok; uint32_t elapsed; };
  static void worker(void *arg);
  Result fetch(const Job &job);
  std::vector<runtime_image::RuntimeImage*> images_;
  CallbackManager<void(int,bool)> callbacks_;
  QueueHandle_t jobs_=nullptr, results_=nullptr;
  int selected_=-1;
  uint32_t generation_=0;
  bool busy_=false;
  esp_http_client_handle_t client_=nullptr; // Worker only.
  unsigned sessions_=0, requests_=0; // Worker only.
};
class ResultTrigger : public Trigger<int,bool> {
 public:
  explicit ResultTrigger(Fetcher *parent) {
    parent->add_callback([this](int slot,bool ok){ this->trigger(slot,ok); });
  }
};
}
