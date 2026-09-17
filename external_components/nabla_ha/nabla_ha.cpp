#include "nabla_ha.h"
#include "response_reader.h"
#include "esphome/components/wifi/wifi_component.h"
#include "esphome/components/json/json_util.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "freertos/task.h"
#include <algorithm>
namespace esphome::nabla_ha {
static SemaphoreHandle_t tls_mutex=nullptr;
static const char *const TAG="nabla_ha";
void Client::setup(){
  if(!tls_mutex)tls_mutex=xSemaphoreCreateMutex();
  requests_=xQueueCreate(1,sizeof(Job));results_=xQueueCreate(1,sizeof(Result));
  if(!tls_mutex||!requests_||!results_||
     xTaskCreate(worker,"nabla_ha",8192,this,1,nullptr)!=pdPASS){
    mark_failed();set_status(Status::INVALID);
  }
}
void Client::dump_config(){
  ESP_LOGI(TAG,"%s: connection state %u, %u entities",url_.c_str(),unsigned(status_),unsigned(entities_.size()));
}
void Client::set_status(Status s){
  if(status_!=s){status_=s;dirty_=true;ESP_LOGI(TAG,"%s: connection state %u",url_.c_str(),unsigned(s));}
  if(s!=Status::READY){for(auto &v:states_)v.available=false;pending_.fill(false);command_waiting_=false;}
}
bool Client::available(int i) const {
  return i>=0&&i<int(entities_.size())&&ready()&&seen_[i]&&
    millis()-seen_[i]<30000&&states_[i].available&&!pending_[i];
}
bool Client::command(int i,bool on,int level){
  if(!available(i)||command_waiting_||level<0||level>100)return false;
  queued_command_={i,true,on,level,states_[i].dimmable};command_waiting_=true;
  pending_[i]=true;dirty_=true;return true;
}
void Client::loop(){
  if(is_failed())return;
  Result r;
  if(xQueueReceive(results_,&r,0)==pdTRUE){
    busy_=false;if(r.command){pending_[r.slot]=false;dirty_=true;}set_status(r.status);
    if(r.valid){
      auto &old=states_[r.slot];
      if(old.on!=r.state.on||old.available!=r.state.available||
         old.dimmable!=r.state.dimmable||old.brightness!=r.state.brightness)dirty_=true;
      old=r.state;seen_[r.slot]=millis();
    } else {states_[r.slot].available=false;dirty_=true;}
    next_=millis()+(ready()?600:8000);
  }
  if(!wifi::global_wifi_component->is_connected()){set_status(Status::UNREACHABLE);return;}
  for(int i=0;i<int(entities_.size());i++)
    if(states_[i].available&&millis()-seen_[i]>=30000){states_[i].available=false;dirty_=true;}
  if(!busy_&&command_waiting_){
    command_waiting_=false;
    if(ready()&&states_[queued_command_.slot].available&&millis()-seen_[queued_command_.slot]<30000){
      if(xQueueSend(requests_,&queued_command_,0)==pdTRUE)busy_=true;
    }else {pending_[queued_command_.slot]=false;dirty_=true;}
  }
  if(!busy_&&int32_t(millis()-next_)>=0){
    Job j{cursor_,false,false,0};cursor_=(cursor_+1)%entities_.size();
    if(xQueueSend(requests_,&j,0)==pdTRUE)busy_=true;
  }
}
void Client::worker(void *arg){
  auto *self=static_cast<Client*>(arg);Job job;
  while(true)if(xQueueReceive(self->requests_,&job,portMAX_DELAY)==pdTRUE){
    xSemaphoreTake(tls_mutex,portMAX_DELAY);
    Result r=self->perform(job);
    xSemaphoreGive(tls_mutex);
    xQueueSend(self->results_,&r,portMAX_DELAY);
  }
}
bool Client::request(const std::string &path,const std::string &body,std::string &out,int &code){
  std::string url=url_+path,auth="Bearer "+token_;
  esp_http_client_config_t cfg{};
  cfg.url=url.c_str();cfg.timeout_ms=3000;cfg.crt_bundle_attach=esp_crt_bundle_attach;
  cfg.disable_auto_redirect=true;cfg.buffer_size=1024;cfg.buffer_size_tx=1024;
  auto client=esp_http_client_init(&cfg);
  if(!client)return false;
  esp_http_client_set_header(client,"Authorization",auth.c_str());
  esp_http_client_set_header(client,"Content-Type","application/json");
  esp_http_client_set_method(client,body.empty()?HTTP_METHOD_GET:HTTP_METHOD_POST);
  bool ok=esp_http_client_open(client,body.size())==ESP_OK;
  if(ok&&!body.empty())ok=esp_http_client_write(client,body.data(),body.size())==int(body.size());
  if(ok){
    int64_t length=esp_http_client_fetch_headers(client);
    code=esp_http_client_get_status_code(client);
    if(length>8192)ok=false;
    if(ok)ok=read_response(out,
      [client](char *buffer,size_t size){return esp_http_client_read(client,buffer,size);},
      [client](){return esp_http_client_is_complete_data_received(client);},
      [](){return millis();});
  }
  esp_http_client_close(client);esp_http_client_cleanup(client);
  return ok;
}
Client::Result Client::perform(const Job &job){
  Result r{job.slot,Status::UNREACHABLE,{},false,job.command};
  std::string out;int code=0;
  const auto &entity=entities_[job.slot];
  if(job.command){
    JsonDocument doc;doc["entity_id"]=entity;
    bool light=entity.compare(0,6,"light.")==0;
    if(light&&job.on&&job.dimmable)doc["brightness_pct"]=job.level;
    std::string body;serializeJson(doc,body);
    if(!request("/api/services/"+std::string(light?"light":"switch")+
       (job.on?"/turn_on":"/turn_off"),body,out,code))return r;
    if(code==401||code==403){r.status=Status::UNAUTHORIZED;return r;}
    if(code!=200){ESP_LOGW(TAG,"%s: HTTP %d",url_.c_str(),code);r.status=Status::INVALID;return r;}
  }
  if(!request("/api/states/"+entity,"",out,code))return r;
  if(code==401||code==403){r.status=Status::UNAUTHORIZED;return r;}
  if(code==404){r.status=Status::READY;return r;}
  if(code!=200){ESP_LOGW(TAG,"%s: HTTP %d",url_.c_str(),code);r.status=Status::INVALID;return r;}
  JsonDocument doc;
  if(deserializeJson(doc,out)||!doc["entity_id"].is<const char*>()||
     entity!=doc["entity_id"].as<std::string>()||!doc["state"].is<const char*>()){
    ESP_LOGW(TAG,"%s: invalid state JSON (%u bytes)",url_.c_str(),unsigned(out.size()));
    r.status=Status::INVALID;return r;
  }
  std::string state=doc["state"].as<std::string>();
  r.state.on=state=="on";r.state.available=state=="on"||state=="off";
  r.state.brightness=std::clamp(int(doc["attributes"]["brightness"]|0),0,255)*100/255;
  for(JsonVariant mode:doc["attributes"]["supported_color_modes"].as<JsonArray>())
    if(mode.is<const char*>()&&std::string(mode.as<const char*>())!="onoff"&&
       std::string(mode.as<const char*>())!="unknown")r.state.dimmable=true;
  r.valid=true;r.status=Status::READY;return r;
}
}
