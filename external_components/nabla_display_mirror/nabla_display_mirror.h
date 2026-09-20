// Tee the logical monochrome pixels to the real display and a bounded web snapshot.
#pragma once
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/helpers.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include "mirror_page.h"
#include <array>
#include <mutex>
namespace esphome::nabla_display_mirror {
class Surface : public display::DisplayBuffer {
 public:
  void update() override {}
  display::DisplayType get_display_type() override {return display::DISPLAY_TYPE_BINARY;}
  void begin(display::Display &target){target_=&target;pixels.fill(0);}
  std::array<uint8_t,1024> pixels{};
 protected:
  int get_width_internal() override {return target_?target_->get_width():128;}
  int get_height_internal() override {return target_?target_->get_height():64;}
  void draw_absolute_pixel_internal(int x,int y,Color color) override {
    if(!target_)return;
    target_->draw_pixel_at(x,y,color);
    if(x<0 || y<0 || x>=128 || y>=64)return;
    const int bit=y*128+x;const uint8_t mask=0x80>>(bit%8);
    if(color.is_on())pixels[bit/8]|=mask;else pixels[bit/8]&=~mask;
  }
  display::Display *target_=nullptr;
};
class Mirror : public Component,public AsyncWebHandler {
 public:
  void set_serve_root(bool value){serve_root_=value;}
  void set_allow_input(bool value){allow_input_=value;}
  Trigger<std::string> *get_action_trigger(){return &trigger_;}
  float get_setup_priority() const override {return setup_priority::WIFI+2;}
  void setup() override {
    token_=str_sprintf("%08x%08x",random_uint32(),random_uint32());
    auto *base=web_server_base::global_web_server_base;base->init();base->add_handler(this);
  }
  display::Display &begin(display::Display &target){
    compatible_=target.get_width()==128 && target.get_height()==64 && target.get_display_type()==display::DISPLAY_TYPE_BINARY;
    if(!compatible_)return target;
    surface_.begin(target);return surface_;
  }
  void end(){
    if(!compatible_)return;
    std::lock_guard<std::mutex> lock(mutex_);snapshot_=surface_.pixels;ready_=true;
  }
  bool canHandle(AsyncWebServerRequest *r) const override {
    auto url=r->url();return (serve_root_ && url=="/") || url=="/mirror" || url=="/mirror/frame" || url=="/mirror/token" || (allow_input_ && url=="/mirror/action") || url=="/mirror/capabilities";
  }
  void handleRequest(AsyncWebServerRequest *r) override {
    const auto url=r->url();
    if(url=="/mirror/capabilities"){r->send(200,"application/json",allow_input_?"{\"input\":true}":"{\"input\":false}");return;}
    if(url=="/mirror/action" && !allow_input_){r->send(404);return;}
    if(r->method()==HTTP_GET && (url=="/" || url=="/mirror")){r->send(200,"text/html; charset=utf-8",MIRROR_PAGE);return;}
    if(r->method()==HTTP_GET && url=="/mirror/token"){
      auto *response=r->beginResponse(200,"text/plain",token_);response->addHeader("Cache-Control","no-store");r->send(response);return;
    }
    if(r->method()==HTTP_GET && url=="/mirror/frame"){
      std::array<uint8_t,1024> frame;
      {std::lock_guard<std::mutex> lock(mutex_);if(!ready_){r->send(409);return;}frame=snapshot_;}
      auto *response=r->beginResponse(200,"application/octet-stream",frame.data(),frame.size());
      response->addHeader("Cache-Control","no-store");r->send(response);return;
    }
    if(r->method()!=HTTP_POST || url!="/mirror/action"){r->send(400);return;}
    auto token=r->get_header("X-Nabla-Token");if(!token || *token!=token_){r->send(401);return;}
    auto *p=r->getParam("action");if(!p){r->send(400);return;}auto action=p->value();
    if(action!="up" && action!="down" && action!="enter" && action!="back"){r->send(400);return;}
    {std::lock_guard<std::mutex> lock(mutex_);if(!pending_.empty()){r->send(409);return;}pending_=action;}
    r->send(200,"text/plain","OK");
  }
  void loop() override {
    std::string action;{std::lock_guard<std::mutex> lock(mutex_);action.swap(pending_);}
    if(!action.empty())trigger_.trigger(action);
  }
 protected:
  Surface surface_;std::array<uint8_t,1024> snapshot_{};
  std::mutex mutex_;bool ready_=false,compatible_=false,serve_root_=true,allow_input_=false;std::string token_,pending_;
  Trigger<std::string> trigger_;
};
}
