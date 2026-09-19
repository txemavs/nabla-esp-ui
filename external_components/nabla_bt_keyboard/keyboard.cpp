// Classic Bluetooth only; never log typed data or automatically select a discovered peer.
#include "keyboard.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_random.h"
#include <cstring>
#include <cstdio>
namespace esphome::nabla_bt_keyboard {
Keyboard *Keyboard::instance_ = nullptr;
void Keyboard::enqueue(const Event &e) {
  if (instance_ && instance_->queue_ && xQueueSend(instance_->queue_, &e, 0)!=pdTRUE) instance_->overflow_=true;
}
void Keyboard::setup() {
  instance_=this;
  preference_=global_preferences->make_preference<std::array<uint8_t,6>>(0x4E424B31);
  std::array<uint8_t,6> saved{};
  if(preference_.load(&saved))memcpy(saved_,saved.data(),6);
  queue_=xQueueCreate(24,sizeof(Event));
  if (!queue_) {mark_failed();return;}
  esp_bt_controller_config_t config=BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  if (esp_bt_controller_init(&config)!=ESP_OK ||
      esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)!=ESP_OK ||
      esp_bluedroid_init()!=ESP_OK || esp_bluedroid_enable()!=ESP_OK) {
    status_="Bluetooth error";mark_failed();return;
  }
  esp_bt_dev_set_device_name("Nabla keyboard host");
  esp_bt_gap_register_callback(gap);
  esp_bt_hid_host_register_callback(hid);
  esp_bt_io_cap_t capability=ESP_BT_IO_CAP_OUT;
  esp_bt_gap_set_security_param(ESP_BT_SP_IOCAP_MODE,&capability,sizeof(capability));
  esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE,ESP_BT_NON_DISCOVERABLE);
  if(esp_bt_hid_host_init()!=ESP_OK){status_="HID error";mark_failed();}
}
void Keyboard::scan() {
  if(!ready_ || connected_ || connecting_ || scanning_)return;
  auto_reconnect_=false;
  count_=0;
  if(saved_[0] || saved_[1] || saved_[2] || saved_[3] || saved_[4] || saved_[5]) {
    memcpy(candidates_[0].address,saved_,6);strcpy(candidates_[0].name,"Saved keyboard");count_=1;
  }
  scanning_=true;status_="Searching";
  if(esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY,8,0)!=ESP_OK){
    scanning_=false;status_="Scan failed";
  }
}
void Keyboard::connect_slot(int slot) {
  if(!available(slot))return;
  memcpy(selected_,candidates_[slot].address,6);
  connecting_=true;started_=millis();pairing_="--";status_="Connecting";
  if(esp_bt_hid_host_connect(selected_)!=ESP_OK){connecting_=false;status_="Connection failed";}
}
void Keyboard::confirm() {
  if(confirmation_){esp_bt_gap_ssp_confirm_reply(selected_,true);confirmation_=false;}
}
void Keyboard::disconnect() {
  auto_reconnect_=false;
  if(connected_ || connecting_)esp_bt_hid_host_disconnect(selected_);
  connecting_=false;connected_=false;boot_=false;confirmation_=false;
  memset(previous_,0,6);pairing_="--";status_="Disconnected";
}
void Keyboard::forget() {
  disconnect();
  esp_err_t result=esp_bt_gap_remove_bond_device(saved_);
  std::array<uint8_t,6> empty{};preference_.save(&empty);global_preferences->sync();memset(saved_,0,6);
  status_=result==ESP_OK?"Bond removal requested":"Bond removal failed";
}
void Keyboard::gap(esp_bt_gap_cb_event_t event,esp_bt_gap_cb_param_t *p) {
  Event e{};
  if(event==ESP_BT_GAP_DISC_RES_EVT) {
    e.kind=1;memcpy(e.address,p->disc_res.bda,6);
    uint32_t cod=0;
    for(int i=0;i<p->disc_res.num_prop;i++){
      auto &prop=p->disc_res.prop[i];
      if(prop.type==ESP_BT_GAP_DEV_PROP_COD && prop.len>=4)memcpy(&cod,prop.val,4);
      if(prop.type==ESP_BT_GAP_DEV_PROP_BDNAME){
        size_t n=std::min(size_t(prop.len),sizeof(e.name)-1);memcpy(e.name,prop.val,n);
      }
      if(prop.type==ESP_BT_GAP_DEV_PROP_EIR){
        uint8_t len=0;auto *name=esp_bt_gap_resolve_eir_data(static_cast<uint8_t*>(prop.val),ESP_BT_EIR_TYPE_CMPL_LOCAL_NAME,&len);
        if(!name)name=esp_bt_gap_resolve_eir_data(static_cast<uint8_t*>(prop.val),ESP_BT_EIR_TYPE_SHORT_LOCAL_NAME,&len);
        if(name)memcpy(e.name,name,std::min(size_t(len),sizeof(e.name)-1));
      }
    }
    // Major peripheral class, keyboard bit in minor class.
    if(((cod>>8)&31)!=5 || !(cod&0x40))return;
    if(!e.name[0])snprintf(e.name,sizeof(e.name),"Keyboard %02X%02X",e.address[4],e.address[5]);
    enqueue(e);
  }else if(event==ESP_BT_GAP_DISC_STATE_CHANGED_EVT && p->disc_st_chg.state==ESP_BT_GAP_DISCOVERY_STOPPED){
    e.kind=2;enqueue(e);
  }else if(event==ESP_BT_GAP_PIN_REQ_EVT){
    e.kind=3;memcpy(e.address,p->pin_req.bda,6);e.value=p->pin_req.min_16_digit?16:6;enqueue(e);
  }else if(event==ESP_BT_GAP_CFM_REQ_EVT){
    e.kind=4;memcpy(e.address,p->cfm_req.bda,6);e.value=p->cfm_req.num_val;enqueue(e);
  }else if(event==ESP_BT_GAP_KEY_NOTIF_EVT){
    e.kind=5;memcpy(e.address,p->key_notif.bda,6);e.value=p->key_notif.passkey;enqueue(e);
  }
}
void Keyboard::hid(esp_hidh_cb_event_t event,esp_hidh_cb_param_t *p) {
  Event e{};
  if(event==ESP_HIDH_INIT_EVT){e.kind=6;e.value=p->init.status;}
  else if(event==ESP_HIDH_OPEN_EVT){e.kind=7;e.value=p->open.status;e.report[0]=p->open.handle;memcpy(e.address,p->open.bd_addr,6);}
  else if(event==ESP_HIDH_CLOSE_EVT){e.kind=8;}
  else if(event==ESP_HIDH_SET_PROTO_EVT){e.kind=9;e.value=p->set_proto.status;}
  else if(event==ESP_HIDH_DATA_IND_EVT){
    if(p->data_ind.status!=ESP_HIDH_OK || p->data_ind.proto_mode!=ESP_HIDH_BOOT_MODE || p->data_ind.len!=8)return;
    e.kind=10;e.value=p->data_ind.handle;memcpy(e.report,p->data_ind.data,8);
  }else return;
  enqueue(e);
}
void Keyboard::loop() {
  if(!queue_)return;
  if(overflow_.exchange(false)){xQueueReset(queue_);disconnect();status_="Input overflow";}
  Event e{};
  for(int n=0;n<24 && xQueueReceive(queue_,&e,0)==pdTRUE;n++){
    if(e.kind==1 && scanning_){
      bool known=false;for(int i=0;i<count_;i++)known|=!memcmp(candidates_[i].address,e.address,6);
      if(!known && count_<4){memcpy(candidates_[count_].address,e.address,6);memcpy(candidates_[count_++].name,e.name,40);}
    }else if(e.kind==2){scanning_=false;status_=count_?"Select keyboard":"No keyboard";}
    else if(e.kind>=3 && e.kind<=5){
      if(!connecting_ || memcmp(selected_,e.address,6))continue;
      char code[32];
      if(e.kind==3){
        if(e.value==16){disconnect();status_="16-digit PIN unsupported";continue;}
        uint32_t number=100000+esp_random()%900000;
        snprintf(code,sizeof(code),"%06u",unsigned(number));
        esp_bt_pin_code_t pin{};memcpy(pin,code,6);
        esp_bt_gap_pin_reply(selected_,true,6,pin);
      }else snprintf(code,sizeof(code),"%06u",unsigned(e.value));
      pairing_=code;confirmation_=e.kind==4;
    }else if(e.kind==6){ready_=e.value==ESP_HIDH_OK;status_=ready_?"Ready":"HID failed";}
    else if(e.kind==7){
      if(memcmp(selected_,e.address,6) || !connecting_){esp_bt_hid_host_disconnect(e.address);continue;}
      connecting_=false;connected_=e.value==ESP_HIDH_OK;handle_=e.report[0];
      status_=connected_?"Negotiating":"Connection failed";pairing_="--";
      if(connected_)esp_bt_hid_host_set_protocol(selected_,ESP_HIDH_BOOT_MODE);
    }else if(e.kind==8){reconnect_at_=millis()+5000;connected_=boot_=false;memset(previous_,0,6);status_="Disconnected";}
    else if(e.kind==9){
      boot_=connected_ && e.value==ESP_HIDH_OK;status_=boot_?"Connected":"Unsupported keyboard";
      if(boot_){auto_reconnect_=true;reconnect_attempts_=0;std::array<uint8_t,6> value{};memcpy(value.data(),selected_,6);preference_.save(&value);global_preferences->sync();memcpy(saved_,selected_,6);}
    }
    else if(e.kind==10 && e.value==handle_ && connected_ && boot_){
      bool invalid=false;for(int i=2;i<8;i++)invalid|=e.report[i]>0 && e.report[i]<4;
      if(invalid){memset(previous_,0,6);continue;}
      for(int i=2;i<8;i++){
        uint8_t k=e.report[i];if(!k)continue;
        bool held=false;for(auto old:previous_)held|=old==k;
        if(!held)keys_.call(k,e.report[0]);
      }
      memcpy(previous_,e.report+2,6);
    }
  }
  if(ready_ && auto_reconnect_ && !connected_ && !connecting_ && !scanning_ &&
     reconnect_attempts_<3 && static_cast<int32_t>(millis()-reconnect_at_)>=0 &&
     (saved_[0] || saved_[1] || saved_[2] || saved_[3] || saved_[4] || saved_[5])){
    memcpy(candidates_[0].address,saved_,6);strcpy(candidates_[0].name,"Saved keyboard");count_=1;
    ++reconnect_attempts_;reconnect_at_=millis()+30000;connect_slot(0);
  }
  if(connecting_ && millis()-started_>60000){disconnect();status_="Connection timeout";}
}
} // namespace
