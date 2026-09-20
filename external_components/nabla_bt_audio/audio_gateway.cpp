// Isolated HFP AG control plane. Never opens SCO or records audio automatically.
#include "audio_gateway.h"
#include <algorithm>
#include <cstdio>
#include <cmath>
#include "esphome/core/log.h"
namespace esphome::nabla_bt_audio {
Gateway *Gateway::instance_=nullptr;
void Gateway::setup() {
  instance_=this;
  discoveries_=xQueueCreate(12,sizeof(Discovery));
  if(!discoveries_){mark_failed();return;}
  esp_bt_controller_config_t config=BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  if(esp_bt_controller_init(&config)!=ESP_OK ||
     esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)!=ESP_OK ||
     esp_bluedroid_init()!=ESP_OK || esp_bluedroid_enable()!=ESP_OK ||
     esp_hf_ag_register_callback(callback)!=ESP_OK || esp_hf_ag_init()!=ESP_OK) {
    status_="Bluetooth initialization failed";mark_failed();return;
  }
  esp_bt_gap_register_callback(gap);
  esp_bt_io_cap_t capability=ESP_BT_IO_CAP_NONE;
  esp_bt_gap_set_security_param(ESP_BT_SP_IOCAP_MODE,&capability,sizeof(capability));
  esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE,ESP_BT_NON_DISCOVERABLE);
  esp_timer_create_args_t args{};args.callback=tick;args.name="nabla_tone";
  if(esp_timer_create(&args,&timer_)!=ESP_OK ||
     esp_hf_ag_register_data_callback(incoming,outgoing)!=ESP_OK){
    mark_failed();status_="Audio init failed";return;
  }
  ready_=true;status_="Ready";
}
uint32_t Gateway::outgoing(uint8_t *data,uint32_t length) {
  auto *self=instance_;if(!self || !data)return 0;
  memset(data,0,length);
  int count=self->remaining_.load();
  const int rate=self->rate_.load();
  for(uint32_t i=0;i+1<length && count>0;i+=2,--count){
    // 440 Hz at approximately -30 dBFS, with 10 ms attack/release.
    int done=rate/2-count;float gain=std::min(1.0f,std::min(done,count)/float(rate/100));
    int16_t sample=int16_t(900*gain*std::sin(6.28318530718*440*self->phase_/rate));
    self->phase_=(self->phase_+1)%rate;
    data[i]=sample&255;data[i+1]=(uint16_t(sample)>>8)&255;
  }
  self->remaining_.store(count);return length;
}
void Gateway::test_audio(){
  if(!connected() || tone_requested_)return;
  remaining_=0;tone_requested_=true;tone_started_=millis();status_="Abriendo audio";
  ESP_LOGI("bt_audio","Opening SCO for test tone...");
  esp_hf_ag_volume_control(peer_,ESP_HF_VOLUME_CONTROL_TARGET_SPK,3);
  esp_err_t err=esp_hf_ag_audio_connect(peer_);
  if(err!=ESP_OK){
    tone_requested_=false;status_="Fallo al abrir audio";
    ESP_LOGE("bt_audio","esp_hf_ag_audio_connect failed: 0x%x",err);
  }
}
void Gateway::scan() {
  if(!ready_ || scanning_ || waiting_ || connected())return;
  xQueueReset(discoveries_);count_=0;scanning_=true;scan_started_=millis();
  status_="Searching";
  if(esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY,8,0)!=ESP_OK){
    scanning_=false;status_="Search failed";
  }
}
void Gateway::gap(esp_bt_gap_cb_event_t event,esp_bt_gap_cb_param_t *p) {
  auto *self=instance_;if(!self || !self->discoveries_)return;
  Discovery item{};
  if(event==ESP_BT_GAP_CFM_REQ_EVT){
    item.kind=1;item.value=p->cfm_req.num_val;memcpy(item.address,p->cfm_req.bda,6);
    xQueueSend(self->discoveries_,&item,0);return;
  }
  if(event==ESP_BT_GAP_PIN_REQ_EVT){
    item.kind=2;memcpy(item.address,p->pin_req.bda,6);
    xQueueSend(self->discoveries_,&item,0);return;
  }
  if(event==ESP_BT_GAP_AUTH_CMPL_EVT){
    item.kind=3;item.value=p->auth_cmpl.stat;memcpy(item.address,p->auth_cmpl.bda,6);
    xQueueSend(self->discoveries_,&item,0);return;
  }
  if(event==ESP_BT_GAP_DISC_STATE_CHANGED_EVT &&
     p->disc_st_chg.state==ESP_BT_GAP_DISCOVERY_STOPPED) item.done=true;
  else if(event==ESP_BT_GAP_DISC_RES_EVT){
    memcpy(item.address,p->disc_res.bda,6);
    // Discovery lists candidates only; a name is not proof of HFP support.
    for(int i=0;i<p->disc_res.num_prop;i++){
      auto &prop=p->disc_res.prop[i];
      if(prop.type==ESP_BT_GAP_DEV_PROP_BDNAME && prop.len>0)
        memcpy(item.name,prop.val,std::min(size_t(prop.len),sizeof(item.name)-1));
      if(prop.type==ESP_BT_GAP_DEV_PROP_EIR){
        uint8_t length=0;
        auto *name=esp_bt_gap_resolve_eir_data(static_cast<uint8_t*>(prop.val),
          ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME,&length);
        if(!name)name=esp_bt_gap_resolve_eir_data(static_cast<uint8_t*>(prop.val),
          ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME,&length);
        if(name)memcpy(item.name,name,std::min(size_t(length),sizeof(item.name)-1));
      }
    }
    if(!item.name[0])snprintf(item.name,sizeof(item.name),"Bluetooth %02X%02X",item.address[4],item.address[5]);
  }else return;
  xQueueSend(self->discoveries_,&item,0);
}
void Gateway::select(size_t slot) {
  if(!available(slot))return;
  memcpy(peer_,candidates_[slot].address,6);connect();
}
void Gateway::confirm() {
  if(!confirm_ || !waiting_)return;
  confirm_=false;esp_bt_gap_ssp_confirm_reply(peer_,true);status_="Emparejando";
}
void Gateway::connect() {
  if(!ready_ || scanning_ || waiting_ || connected())return;
  bool specified=false;for(auto b:peer_)specified|=b!=0;
  if(!specified){status_="Select a peer first";return;}
  waiting_=true;started_=millis();status_="Connecting";
  if(esp_hf_ag_slc_connect(peer_)!=ESP_OK){waiting_=false;status_="Connection failed";}
}
void Gateway::disconnect() {
  if(!ready_)return;
  if(scanning_){esp_bt_gap_cancel_discovery();scanning_=false;}
  esp_hf_ag_slc_disconnect(peer_);waiting_=false;
  confirm_=false;state_=ESP_HF_CONNECTION_STATE_DISCONNECTED;status_="Desconectado";
}
void Gateway::callback(esp_hf_cb_event_t event,esp_hf_cb_param_t *p) {
  auto *self=instance_;if(!self)return;
  if(event==ESP_HF_AUDIO_STATE_EVT && !memcmp(p->audio_stat.remote_addr,self->peer_,6)){
    self->audio_event_=p->audio_stat.state;
    ESP_LOGI("bt_audio","Audio event: state=%d handle=0x%04x frame_size=%u",
             p->audio_stat.state,p->audio_stat.sync_conn_handle,p->audio_stat.preferred_frame_size);
  }
  if(event==ESP_HF_CIND_RESPONSE_EVT && !memcmp(p->cind_rep.remote_addr,self->peer_,6)){
    esp_hf_ag_cind_response(self->peer_,ESP_HF_CALL_STATUS_NO_CALLS,
      ESP_HF_CALL_SETUP_STATUS_IDLE,ESP_HF_NETWORK_STATE_NOT_AVAILABLE,0,
      ESP_HF_ROAMING_STATUS_INACTIVE,0,ESP_HF_CALL_HELD_STATUS_NONE);
  }
  if(event==ESP_HF_COPS_RESPONSE_EVT && !memcmp(p->cops_rep.remote_addr,self->peer_,6)){
    char name[]="Nabla";esp_hf_ag_cops_response(self->peer_,name);
  }
  if(event==ESP_HF_CONNECTION_STATE_EVT &&
     !memcmp(p->conn_stat.remote_bda,self->peer_,6))
    self->pending_.store(p->conn_stat.state);
}
void Gateway::loop() {
  int audio=audio_event_.exchange(-1);
  if(audio>=0)ESP_LOGI("bt_audio","Audio state=%d",audio);
  if(audio==ESP_HF_AUDIO_STATE_CONNECTED || audio==ESP_HF_AUDIO_STATE_CONNECTED_MSBC){
    if(tone_requested_){
      rate_=audio==ESP_HF_AUDIO_STATE_CONNECTED_MSBC?16000:8000;
      remaining_=rate_.load()/2;tone_running_=true;tone_started_=millis();
      status_="Tono de prueba";esp_timer_start_periodic(timer_,7500);
    }else esp_hf_ag_audio_disconnect(peer_);
  }
  if(tone_requested_ && ((tone_running_ && remaining_.load()==0) ||
      millis()-tone_started_>(tone_running_?1500u:10000u))){
    const bool sent=tone_running_ && remaining_.load()==0;
    const bool opened=tone_running_;
    esp_timer_stop(timer_);remaining_=0;tone_requested_=tone_running_=false;
    esp_hf_ag_audio_disconnect(peer_);
    status_=sent?"Tono enviado":(opened?"Audio interrumpido":"Tiempo de apertura agotado");
    ESP_LOGI("bt_audio","Tone result: opened=%d samples_consumed=%d",opened,sent);
  }
  if(audio==ESP_HF_AUDIO_STATE_DISCONNECTED){
    bool failed=tone_requested_ && !tone_running_;
    esp_timer_stop(timer_);remaining_=0;tone_requested_=tone_running_=false;
    status_=failed?"No se pudo abrir audio":"Audio cerrado";
  }
  Discovery item{};
  if(discoveries_) for(int n=0;n<12 && xQueueReceive(discoveries_,&item,0)==pdTRUE;n++){
    if(item.kind){
      if(memcmp(item.address,peer_,6))continue;
      if(item.kind==1 && waiting_){
        confirm_=true;pairing_status_="Confirmar "+std::to_string(item.value);
        status_=pairing_status_.c_str();
      }else if(item.kind==2){
        esp_bt_pin_code_t pin{};esp_bt_gap_pin_reply(item.address,false,0,pin);
        status_="PIN antiguo no soportado";
      }else if(item.kind==3){
        ESP_LOGI("bt_audio","Authentication status=%u",unsigned(item.value));
        if(item.value!=ESP_BT_STATUS_SUCCESS){disconnect();status_="Fallo de autenticacion";}
      }
      continue;
    }
    if(item.done){scanning_=false;status_=count_?"Candidates found":"No devices";continue;}
    if(!scanning_)continue;
    size_t slot=0;while(slot<count_ && memcmp(candidates_[slot].address,item.address,6))++slot;
    if(slot<candidates_.size()){candidates_[slot]=item;if(slot==count_)++count_;ESP_LOGI("bt_audio","Candidate %u: %s",unsigned(slot+1),item.name);}
  }
  if(scanning_ && millis()-scan_started_>15000){
    esp_bt_gap_cancel_discovery();scanning_=false;status_="Search ended";
  }
  int next=pending_.exchange(-1);
  if(next>=0) {
    state_=next;ESP_LOGI("bt_audio","HFP state=%d",next);
    if(connected()){waiting_=false;confirm_=false;status_="HFP conectado (sin audio)";}
    else if(next==ESP_HF_CONNECTION_STATE_DISCONNECTED){waiting_=false;status_="Disconnected";}
  }
  if(waiting_ && millis()-started_>30000){disconnect();status_="Connection timeout";}
}
} // namespace
