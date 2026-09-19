// Runtime device/network information helpers; supplies semantic information fields to shared views.

#pragma once
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"
#include "esphome/core/version.h"
#include "esphome/components/nabla_navigation/info.h"
#ifdef USE_WIFI
#include "esphome/components/wifi/wifi_component.h"
#ifdef USE_ESP32
#include "esp_netif.h"
#endif
#endif
namespace nabla_info {
struct Labels {
  const char *disconnected, *unavailable, *chip, *screen, *color, *mono, *router, *mask, *signal;
};
inline void refresh(const char *model, const char *display, const char *dimensions,
                    bool monochrome, bool preview, const Labels &l) {
  values[1] = std::string(model).substr(0, 64);
  std::string chip = l.unavailable;
#ifdef ESPHOME_VARIANT
  chip = ESPHOME_VARIANT;
#elif defined(USE_HOST)
  chip = "Host / SDL";
#endif
  values[2] = l.unavailable;
  values[3] = l.unavailable;
  std::string router = l.unavailable, mask = l.unavailable, signal = l.unavailable;
#ifdef USE_WIFI
  auto *wifi = esphome::wifi::global_wifi_component;
  if (wifi && wifi->is_connected()) {
    char ssid[esphome::wifi::SSID_BUFFER_SIZE];
    values[2] = wifi->wifi_ssid_to(ssid);
    for (char &c : values[2]) if (static_cast<unsigned char>(c) < 32) c = ' ';
    char address[esphome::network::IP_ADDRESS_BUFFER_SIZE];
    values[3] = wifi->get_ip_addresses()[0].str_to(address);
    signal = std::to_string(wifi->wifi_rssi()) + " dBm";
#ifdef USE_ESP32
    esp_netif_ip_info_t ip{};
    auto *netif = wifi->get_esp_netif_sta();
    if (netif && esp_netif_get_ip_info(netif, &ip) == ESP_OK) {
      char text[16];
      snprintf(text, sizeof(text), IPSTR, IP2STR(&ip.gw));
      router = text;
      snprintf(text, sizeof(text), IPSTR, IP2STR(&ip.netmask));
      mask = text;
    }
#endif
  } else {
    values[2] = l.disconnected;
  }
#endif
#ifdef USE_HOST
  // Explicit host-only fixture: documentation addresses, never a saved network.
  if (preview) {
    chip = "ESP32-S3 (demo)";
    values[2] = "Wi-Fi demo";
    values[3] = "192.0.2.42";
    router = "192.0.2.1";
    mask = "255.255.255.0";
    signal = "-57 dBm (demo)";
  }
#endif
  details[1] = values[1] + "\n" + l.chip + ": " + chip + "\n" +
      l.screen + ": " + std::string(display).substr(0, 64) + "\n" +
      dimensions + " px - " + (monochrome ? l.mono : l.color);
  details[2] = "Wi-Fi: " + values[2] + "\nIP: " + values[3] + "\n" +
      l.router + ": " + router + "\n" + l.mask + ": " + mask + "\n" + l.signal + ": " + signal;
  if (values[3] != l.unavailable) values[2] += " - " + values[3];
  values[4] = ESPHOME_VERSION;
  const uint32_t seconds = (esphome::millis)() / 1000;
  char uptime[32];
  snprintf(uptime, sizeof(uptime), "%luh %02lum %02lus",
           static_cast<unsigned long>(seconds / 3600),
           static_cast<unsigned long>((seconds / 60) % 60),
           static_cast<unsigned long>(seconds % 60));
  values[5] = uptime;
}
}
