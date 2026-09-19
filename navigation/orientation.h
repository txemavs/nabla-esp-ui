// Orientation and focus-order helpers; keeps navigation aligned with the logical rotated viewport.

#pragma once
#include <vector>
#include <algorithm>
namespace nabla_orientation {
struct Reader { lv_indev_t *device; lv_indev_read_cb_t read; };
inline std::vector<Reader> readers;
inline void read(lv_indev_t *device, lv_indev_data_t *data) {
  for (const auto &entry : readers) if (entry.device == device) {
    entry.read(device, data);
    // ESPHome leaves the previous point untouched on release. After rotation
    // that point can be outside the new bounds; pressed coordinates are valid.
    if (data->state == LV_INDEV_STATE_RELEASED) {
      auto *display = lv_indev_get_display(device);
      data->point.x = std::clamp<int32_t>(data->point.x, 0, lv_display_get_horizontal_resolution(display) - 1);
      data->point.y = std::clamp<int32_t>(data->point.y, 0, lv_display_get_vertical_resolution(display) - 1);
    }
    return;
  }
}
inline void prepare() {
  for (auto *device = lv_indev_get_next(nullptr); device; device = lv_indev_get_next(device)) {
    if (lv_indev_get_type(device) != LV_INDEV_TYPE_POINTER || lv_indev_get_read_cb(device) == read) continue;
    auto callback = lv_indev_get_read_cb(device);
    if (!callback) continue;
    readers.push_back({device, callback});
    lv_indev_set_read_cb(device, read);
  }
}
}
