#pragma once
#include "esphome/core/component.h"
#include <array>
#include <string>
#include <vector>
#include "esp_http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
namespace esphome::nabla_ha {
struct State {
  bool on=false, available=false, dimmable=false;
  int brightness=0;
};
enum class Status : uint8_t { CONNECTING, READY, UNREACHABLE, UNAUTHORIZED, INVALID };
class Client : public Component {
 public:
  void set_url(const std::string &v){url_=v;}
  void set_token(const std::string &v){token_=v;}
  void add_entity(const std::string &v){entities_.push_back(v);}
  void setup() override;
  void loop() override;
  void dump_config() override;
  bool ready() const {return status_==Status::READY;}
  bool available(int i) const;
  bool pending(int i) const {return i>=0&&i<8&&pending_[i];}
  State state(int i) const {return i>=0&&i<int(entities_.size())?states_[i]:State{};}
  Status status() const {return status_;}
  const std::string &url() const {return url_;}
  bool consume_dirty(){bool v=dirty_;dirty_=false;return v;}
  bool command(int slot,bool on,int brightness);
 protected:
  struct Job {int slot;bool command;bool on;int level;bool dimmable=false;};
  struct Result {int slot;Status status;State state;bool valid;bool command;};
  static void worker(void *arg);
  Result perform(const Job &job);
  bool request(const std::string &path,const std::string &body,std::string &out,int &code);
  void set_status(Status value);
  std::string url_,token_;
  std::vector<std::string> entities_;
  std::array<State,8> states_{};
  std::array<uint32_t,8> seen_{};
  std::array<bool,8> pending_{};
  QueueHandle_t requests_=nullptr,results_=nullptr;
  Status status_=Status::CONNECTING;
  bool busy_=false,dirty_=true,command_waiting_=false;
  Job queued_command_{};
  uint32_t next_=0;
  int cursor_=0;
  // Worker-owned. UI/main loop must never access the transport.
  esp_http_client_handle_t http_=nullptr;
  unsigned request_count_=0, connection_count_=0;
};
}
