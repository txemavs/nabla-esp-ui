#pragma once
#include <cmath>

namespace nabla_logo {
inline void planar_frame(void *object, int32_t angle) {
  lv_image_set_rotation(static_cast<lv_obj_t *>(object), angle);
}
inline void turn(lv_obj_t *image, bool left) {
  if (!image) return;
  const int target = left ? 900 : 0;
  // Do not restart a running transition on unrelated UI refreshes.
  lv_anim_t *running = lv_anim_get(image, planar_frame);
  if (running && running->end_value == target) return;
  if (!running && lv_image_get_rotation(image) == target) return;
  lv_anim_delete(image, planar_frame);
  lv_obj_update_layout(image);
  lv_image_set_pivot(image, lv_obj_get_width(image) / 2, lv_obj_get_height(image) / 2);
  lv_anim_t animation;
  lv_anim_init(&animation);
  lv_anim_set_var(&animation, image);
  lv_anim_set_values(&animation, lv_image_get_rotation(image), target);
  lv_anim_set_duration(&animation, 180);
  lv_anim_set_path_cb(&animation, lv_anim_path_ease_in_out);
  lv_anim_set_exec_cb(&animation, planar_frame);
  lv_anim_start(&animation);
}
inline void spindle_frame(void *object, int32_t phase) {
  auto *image = static_cast<lv_obj_t *>(object);
  const float facing = std::cos(phase * 6.28318530718f / 3600.0f);
  // Foreshortening around the vertical centerline. Y never changes:
  // the bottom tip stays anchored throughout the entire turn.
  const int scale = std::max(5, static_cast<int>(256 * std::abs(facing)));
  lv_image_set_scale_x(image, scale);
  lv_obj_set_style_image_recolor(image, lv_color_hex(0x16334C), 0);
  const int shade = phase == 3600 ? 0 : static_cast<int>((1 - facing) * 65);
  lv_obj_set_style_image_recolor_opa(image, shade, 0);
}
inline void spin(lv_obj_t *image) {
  if (!image) return;
  lv_anim_delete(image, spindle_frame);
  lv_obj_update_layout(image);
  lv_image_set_rotation(image, 0);
  lv_image_set_pivot(image, lv_obj_get_width(image) / 2, lv_obj_get_height(image) / 2);
  lv_image_set_scale_x(image, 256);
  lv_anim_t animation;
  lv_anim_init(&animation);
  lv_anim_set_var(&animation, image);
  lv_anim_set_values(&animation, 0, 3600);
  lv_anim_set_duration(&animation, 1000);
  lv_anim_set_path_cb(&animation, lv_anim_path_ease_in_out);
  lv_anim_set_exec_cb(&animation, spindle_frame);
  lv_anim_start(&animation);
}
}
