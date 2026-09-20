// Browser renderer; HTTP callbacks queue bounded changes for the shared Wi-Fi flow.
#pragma once
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include "esphome/components/json/json_util.h"
#include "esphome/components/nabla_navigation/navigation.h"
#include "esphome/components/wifi/wifi_component.h"
#include "wifi_flow.h"
#include "page.h"
#include <mutex>
namespace esphome::nabla_web {
class Web : public Component, public AsyncWebHandler {
 public:
  void set_camera_node(int node){camera_node_=node;}
  void set_camera_port(uint16_t port){port_=port;}
  float get_setup_priority() const override {return setup_priority::WIFI+2;}
  void setup() override {
    token_=str_sprintf("%08x%08x",random_uint32(),random_uint32());
    auto *base=web_server_base::global_web_server_base;
    base->init();base->add_handler(this);
    publish();
  }
  bool canHandle(AsyncWebServerRequest *r) const override {
    return r->url()=="/" || r->url()=="/nabla" || r->url()=="/nabla/state" || r->url()=="/nabla/wifi";
  }
  void handleRequest(AsyncWebServerRequest *r) override {
    if(r->method()==HTTP_GET && (r->url()=="/" || r->url()=="/nabla")){
      r->send(200,"text/html; charset=utf-8",PAGE);return;
    }
    if(r->method()==HTTP_GET && r->url()=="/nabla/state"){
      std::lock_guard<std::mutex> lock(mutex_);
      auto *res=r->beginResponse(200,"application/json",snapshot_);
      res->addHeader("Cache-Control","no-store");r->send(res);return;
    }
    if(r->method()!=HTTP_POST || r->url()!="/nabla/wifi"){r->send(400);return;}
    auto header=r->get_header("X-Nabla-Token");
    if(!header || *header!=token_){r->send(401);return;}
    auto param=[&](const char *name){auto *p=r->getParam(name);return p?p->value():std::string();};
    auto action=param("action"),ssid=param("ssid"),password=param("password"),open=param("open");
    if((action!="scan" && action!="connect" && action!="cancel") || ssid.size()>32 || password.size()>63){
      r->send(400);return;
    }
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if(!command_.empty()){r->send(409);return;}
      command_=action;ssid_=ssid;password_=password;open_=open=="true";
    }
    std::fill(password.begin(),password.end(),0);
    r->send(200,"application/json","{}");
  }
  void loop() override {
    std::string action,ssid,password;bool open=false;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      action.swap(command_);ssid.swap(ssid_);password.swap(password_);open=open_;
    }
    if(action=="scan")flow_.scan(millis());
    if(action=="cancel")flow_.clear();
    if(action=="connect" && flow_.stage!=nabla_forms::Stage::CONNECTING && flow_.stage!=nabla_forms::Stage::SCANNING){
      flow_.clear();flow_.ssid=ssid;flow_.password=password;flow_.open=open;flow_.connect(millis());
    }
    std::fill(password.begin(),password.end(),0);
    if(!action.empty() || millis()-published_>=1000)publish();
  }
 protected:
  void publish(){
    published_=millis();
    auto value=json::build_json([&](JsonObject root){
      root["camera_node"]=camera_node_;root["token"]=token_;root["camera_port"]=port_;
      root["stage"]=int(flow_.stage);root["error"]=int(flow_.error);
      root["uptime"]=millis()/1000;
      auto *wifi=wifi::global_wifi_component;
      root["connected"]=wifi->is_connected();
      char name[wifi::SSID_BUFFER_SIZE];char address[network::IP_ADDRESS_BUFFER_SIZE];
      root["ssid"]=wifi->is_connected()?wifi->wifi_ssid_to(name):"";
      root["ip"]=wifi->get_ip_addresses()[0].str_to(address);
      auto menu=root["nodes"].to<JsonArray>();
      for(int i=0;i<nabla::count;++i){
        auto node=menu.add<JsonObject>();node["id"]=i;node["parent"]=nabla::nodes[i].parent;
        node["action"]=nabla::nodes[i].action;node["title"]=nabla::nodes[i].title;node["detail"]=nabla::detail(i);
      }
      auto networks=root["networks"].to<JsonArray>();
      for(int i=0;i<flow_.results();++i){
        auto n=networks.add<JsonObject>();auto &v=flow_.result(i);
        n["ssid"]=v.ssid;n["open"]=v.open;n["rssi"]=v.rssi;
      }
    });
    std::lock_guard<std::mutex> lock(mutex_);snapshot_=std::move(value);
  }
  int camera_node_=1;uint16_t port_=8080;uint32_t published_=0;
  nabla_forms::WifiFlow flow_;
  std::mutex mutex_;std::string snapshot_,token_,command_,ssid_,password_;bool open_=false;
};
}
