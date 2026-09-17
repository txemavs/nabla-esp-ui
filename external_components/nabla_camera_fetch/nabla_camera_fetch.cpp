#include "nabla_camera_fetch.h"
#include "esphome/components/wifi/wifi_component.h"
#include "esphome/core/log.h"
#include "esp_crt_bundle.h"
#include "esp_heap_caps.h"
#include <cstring>
#include <algorithm>
namespace esphome::nabla_camera_fetch {
static const char *const TAG="camera_fetch";
void Fetcher::setup() {
  jobs_=xQueueCreate(1,sizeof(Job));
  results_=xQueueCreate(1,sizeof(Result));
  if(!jobs_ || !results_ || xTaskCreate(worker,"camera_https",8192,this,1,nullptr)!=pdPASS) {
    mark_failed(); return;
  }
}
bool Fetcher::request(int slot,const std::string &url) {
  if(is_failed() || busy_ || selected_!=slot || slot<0 || slot>=int(images_.size()) ||
     url.size()>=sizeof(Job::url) || url.compare(0,8,"https://")!=0 ||
     !wifi::global_wifi_component->is_connected())return false;
  Job job{};job.slot=slot;job.generation=generation_;
  std::memcpy(job.url,url.c_str(),url.size()+1);
  if(xQueueSend(jobs_,&job,0)!=pdTRUE)return false;
  busy_=true;return true;
}
void Fetcher::loop() {
  if(is_failed())return;
  Result result{};
  if(xQueueReceive(results_,&result,0)!=pdTRUE)return;
  busy_=false;
  if(result.generation!=generation_ || result.slot!=selected_) {
    heap_caps_free(result.data);return;
  }
  const uint32_t started=millis();
  bool ok=result.ok;
  auto *image=images_[result.slot];
  if(ok) {
    ok=image->begin_decode(result.size);
    if(ok) {
      const int consumed=image->feed_data(result.data,result.size);
      const bool complete=consumed>=0 && image->is_decode_finished();
      const bool ended=image->end_decode();
      ok=complete && ended;
    }
  }
  heap_caps_free(result.data);
  ESP_LOGD(TAG,"slot=%d bytes=%u network=%u ms decode=%u ms ok=%d",
    result.slot,unsigned(result.size),unsigned(result.elapsed),unsigned(millis()-started),ok);
  callbacks_.call(result.slot,ok);
}
void Fetcher::worker(void *arg) {
  auto *self=static_cast<Fetcher*>(arg);Job job{};
  for(;;) {
    if(xQueueReceive(self->jobs_,&job,portMAX_DELAY)!=pdTRUE)continue;
    Result result=self->fetch(job);
    xQueueSend(self->results_,&result,portMAX_DELAY);
  }
}
Fetcher::Result Fetcher::fetch(const Job &job) {
  const uint32_t started=millis();
  Result result{job.slot,job.generation,nullptr,0,false,0};
  if(!client_) {
    esp_http_client_config_t cfg{};
    cfg.url=job.url;cfg.timeout_ms=4000;cfg.crt_bundle_attach=esp_crt_bundle_attach;
    cfg.keep_alive_enable=true;cfg.disable_auto_redirect=true;
    cfg.buffer_size=1024;cfg.buffer_size_tx=1536;
    client_=esp_http_client_init(&cfg);++sessions_;
  }
  bool ok=client_ && esp_http_client_set_url(client_,job.url)==ESP_OK;
  if(ok) {
    esp_http_client_set_method(client_,HTTP_METHOD_GET);
    esp_http_client_set_header(client_,"Accept","image/jpeg");
    ok=esp_http_client_open(client_,0)==ESP_OK;
  }
  int64_t length=-1;
  if(ok) {
    length=esp_http_client_fetch_headers(client_);
    ok=length>=0 && length<=int64_t(LIMIT) &&
       esp_http_client_get_status_code(client_)==200;
  }
  if(ok) {
    // Compressed bytes live in PSRAM, outside the internal TLS heap.
    result.data=static_cast<uint8_t*>(heap_caps_malloc(length>0?size_t(length):LIMIT,
                                                     MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT));
    ok=result.data!=nullptr;
  }
  const size_t capacity=length>0?size_t(length):LIMIT;
  while(ok && !esp_http_client_is_complete_data_received(client_)) {
    if(result.size>=capacity || uint32_t(millis()-started)>6000){ok=false;break;}
    const int read=esp_http_client_read(client_,reinterpret_cast<char*>(result.data+result.size),
                                       std::min(size_t(4096),capacity-result.size));
    if(read<0){ok=false;break;}
    if(read==0 && !esp_http_client_is_complete_data_received(client_)) {
      vTaskDelay(pdMS_TO_TICKS(10));continue;
    }
    result.size+=read;
  }
  ok=ok && result.size>=2 && result.data[0]==0xFF && result.data[1]==0xD8;
  if(client_ && (!ok || !esp_http_client_is_persistent_connection(client_))) {
    esp_http_client_close(client_);esp_http_client_cleanup(client_);client_=nullptr;
  }
  result.ok=ok;result.elapsed=millis()-started;
  if(++requests_%8==0)
    ESP_LOGI(TAG,"requests=%u sessions=%u last=%u ms bytes=%u internal_free=%u",
      requests_,sessions_,unsigned(result.elapsed),unsigned(result.size),
      unsigned(heap_caps_get_free_size(MALLOC_CAP_INTERNAL)));
  return result;
}
}
