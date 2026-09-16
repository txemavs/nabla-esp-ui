#pragma once
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"
#include "esphome/core/version.h"
#include "esphome/components/nabla_navigation/info.h"
#ifdef USE_WIFI
#include "esphome/components/wifi/wifi_component.h"
#endif
namespace nabla_info {
inline void refresh(const char *model, const char *disconnected, const char *unavailable) {
  values[1] = std::string(model).substr(0, 64);
  values[2] = unavailable;
  values[3] = unavailable;
#ifdef USE_WIFI
  auto *wifi = esphome::wifi::global_wifi_component;
  if (wifi && wifi->is_connected()) {
    char ssid[esphome::wifi::SSID_BUFFER_SIZE];
    values[2] = wifi->wifi_ssid_to(ssid);
    for (char &c : values[2]) if (static_cast<unsigned char>(c) < 32) c = ' ';
    char address[esphome::network::IP_ADDRESS_BUFFER_SIZE];
    values[3] = wifi->get_ip_addresses()[0].str_to(address);
  } else {
    values[2] = disconnected;
  }
#endif
  values[4] = ESPHOME_VERSION;
  const uint32_t seconds = esphome::millis() / 1000;
  char uptime[32];
  snprintf(uptime, sizeof(uptime), "%luh %02lum %02lus",
           static_cast<unsigned long>(seconds / 3600),
           static_cast<unsigned long>((seconds / 60) % 60),
           static_cast<unsigned long>(seconds % 60));
  values[5] = uptime;
}
}
