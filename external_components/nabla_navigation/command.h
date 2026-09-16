#pragma once
#include "esphome/core/automation.h"
#include "navigation.h"
namespace esphome { namespace nabla_navigation {
class CommandTrigger : public Trigger<> {
 public:
  explicit CommandTrigger(int node) : node_(node) {
    nabla::commands[node_].execute = [this]() { this->trigger(); };
  }
  void set_available(std::function<bool()> fn) { nabla::commands[node_].available = fn; }
  void set_state(std::function<std::string()> fn) { nabla::commands[node_].state = fn; }
 protected:
  int node_;
};
}}
