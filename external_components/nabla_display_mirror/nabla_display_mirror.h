// Capture logical pixels through the compact draw pass or the ESPHome LVGL flush callback.
#pragma once
#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/core/helpers.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include "mirror_page.h"
#include <cstring>
#ifdef NABLA_MIRROR_LVGL
#include "esphome/components/lvgl/lvgl_esphome.h"
#endif
#include <mutex>
namespace esphome::nabla_display_mirror {
class Surface : public display::DisplayBuffer {
 public:
  void update() override {}
  display::DisplayType get_display_type() override {return color_?display::DISPLAY_TYPE_COLOR:display::DISPLAY_TYPE_BINARY;}
  void begin(display::Display &target){target_=&target;memset(pixels,0,length);}
  void configure(int w,int h,bool color){width_=w;height_=h;color_=color;length=color?w*h:(w*h+7)/8;pixels=allocator.allocate(length);}
  void pixel(int x,int y,Color color){
    if(!pixels || x<0 || y<0 || x>=width_ || y>=height_)return;
    const int bit=y*width_+x;
    if(color_){pixels[bit]=(color.red&0xe0)|((color.green>>3)&0x1c)|(color.blue>>6);return;}
    const uint8_t mask=0x80>>(bit%8);
    if(color.is_on())pixels[bit/8]|=mask;else pixels[bit/8]&=~mask;
  }
  RAMAllocator<uint8_t> allocator;
  uint8_t *pixels=nullptr;size_t length=0;int width_=128,height_=64;bool color_=false;
 protected:
  int get_width_internal() override {return width_;}
  int get_height_internal() override {return height_;}
  void draw_absolute_pixel_internal(int x,int y,Color color) override {
    if(!target_)return;
    target_->draw_pixel_at(x,y,color);
    pixel(x,y,color);
  }
  display::Display *target_=nullptr;
};
class Mirror : public Component,public AsyncWebHandler {
 public:
  void configure(int w,int h,bool color){width_=w;height_=h;color_=color;}
#ifdef NABLA_MIRROR_LVGL
  void set_lvgl(lvgl::LvglComponent *value){lvgl_=value;}
  static inline Mirror *active_=nullptr;
  static void flush(lv_display_t *display,const lv_area_t *area,uint8_t *data){
    auto *m=active_;
    if(m && m->surface_.pixels){
      const int w=lv_area_get_width(area),h=lv_area_get_height(area);
      for(int y=0;y<h;++y)for(int x=0;x<w;++x){
        uint16_t pixel;memcpy(&pixel,data+2*(y*w+x),2);
#if LV_COLOR_16_SWAP
        pixel=(pixel>>8)|(pixel<<8);
#endif
        m->surface_.pixel(area->x1+x,area->y1+y,Color(((pixel>>11)&31)*255/31,((pixel>>5)&63)*255/63,(pixel&31)*255/31));
      }
      if(lv_display_flush_is_last(display))m->publish();
    }
    lvgl::LvglComponent::static_flush_cb(display,area,data);
  }
#endif
  void set_serve_root(bool value){serve_root_=value;}
  void set_allow_input(bool value){allow_input_=value;}
  Trigger<std::string> *get_action_trigger(){return &trigger_;}
  float get_setup_priority() const override {return setup_priority::WIFI+2;}
  void setup() override {
    surface_.configure(width_,height_,color_);
    snapshot_=allocator_.allocate(surface_.length);
    if(!surface_.pixels || !snapshot_){ESP_LOGE("mirror","Frame buffer allocation failed");mark_failed();return;}
    memset(surface_.pixels,0,surface_.length);
#ifdef NABLA_MIRROR_LVGL
    if(lvgl_){
      auto *display=lvgl_->get_disp();
      if(lv_display_get_horizontal_resolution(display)!=width_ || lv_display_get_vertical_resolution(display)!=height_){mark_failed();return;}
      active_=this;lv_display_set_flush_cb(display,flush);lv_obj_invalidate(lvgl_->get_screen_active());
    }
#endif
    token_=str_sprintf("%08x%08x",random_uint32(),random_uint32());
    auto *base=web_server_base::global_web_server_base;base->init();base->add_handler(this);
  }
  display::Display &begin(display::Display &target){
    compatible_=surface_.pixels && snapshot_ && target.get_width()==width_ && target.get_height()==height_ && (color_ || target.get_display_type()==display::DISPLAY_TYPE_BINARY);
    if(!compatible_)return target;
    surface_.begin(target);return surface_;
  }
  void end(){
    if(!compatible_)return;
    publish();
  }
  void publish(){std::lock_guard<std::mutex> lock(mutex_);memcpy(snapshot_,surface_.pixels,surface_.length);ready_=true;}
  bool canHandle(AsyncWebServerRequest *r) const override {
    auto url=r->url();return (serve_root_ && url=="/") || url=="/mirror" || url=="/mirror/frame" || url=="/mirror/token" || (allow_input_ && url=="/mirror/action") || url=="/mirror/capabilities";
  }
  void handleRequest(AsyncWebServerRequest *r) override {
    const auto url=r->url();
    if(url=="/mirror/capabilities"){auto json=str_sprintf("{\"input\":%s,\"width\":%d,\"height\":%d,\"format\":\"%s\"}",allow_input_?"true":"false",width_,height_,color_?"rgb332":"mono1");r->send(200,"application/json",json.c_str());return;}
    if(url=="/mirror/action" && !allow_input_){r->send(404);return;}
    if(r->method()==HTTP_GET && (url=="/" || url=="/mirror")){r->send(200,"text/html; charset=utf-8",MIRROR_PAGE);return;}
    if(r->method()==HTTP_GET && url=="/mirror/token"){
      auto *response=r->beginResponse(200,"text/plain",token_);response->addHeader("Cache-Control","no-store");r->send(response);return;
    }
    if(r->method()==HTTP_GET && url=="/mirror/frame"){
      auto *frame=allocator_.allocate(surface_.length);if(!frame){r->send(409);return;}
      {std::lock_guard<std::mutex> lock(mutex_);if(!ready_){allocator_.deallocate(frame,surface_.length);r->send(409);return;}memcpy(frame,snapshot_,surface_.length);}
      auto *response=r->beginResponse(200,"application/octet-stream",frame,surface_.length);
      response->addHeader("Cache-Control","no-store");r->send(response);allocator_.deallocate(frame,surface_.length);return;
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
  Surface surface_;RAMAllocator<uint8_t> allocator_;uint8_t *snapshot_=nullptr;
  int width_=128,height_=64;bool color_=false;
#ifdef NABLA_MIRROR_LVGL
  lvgl::LvglComponent *lvgl_=nullptr;
#endif
  std::mutex mutex_;bool ready_=false,compatible_=false,serve_root_=true,allow_input_=false;std::string token_,pending_;
  Trigger<std::string> trigger_;
};
}
