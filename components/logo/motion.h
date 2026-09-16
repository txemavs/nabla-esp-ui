#pragma once
#include <cmath>

namespace nabla_logo {
inline void planar_frame(void *object, int32_t angle) {
  auto *image = static_cast<lv_obj_t *>(object);
  lv_image_set_rotation(image, angle);
  // Keep the visible triangle's bounding-box center at its down-facing
  // position. The SVG canvas is centered on the centroid, not that box.
  // This gives the upward mark a small settling motion without moving its
  // hit target or accumulating offsets when focus changes mid-animation.
  const float w = lv_obj_get_width(image), h = lv_obj_get_height(image);
  const float radians = angle * 3.14159265359f / 1800.0f;
  const float c = std::cos(radians), s = std::sin(radians);
  const float xs[] = {-0.4f * w, 0.4f * w, 0.0f};
  const float ys[] = {-0.2309401f * h, -0.2309401f * h, 0.4618802f * h};
  float min_x = 1e9f, max_x = -1e9f, min_y = 1e9f, max_y = -1e9f;
  for (int i = 0; i < 3; ++i) {
    const float x = xs[i] * c - ys[i] * s;
    const float y = xs[i] * s + ys[i] * c;
    min_x = std::min(min_x, x); max_x = std::max(max_x, x);
    min_y = std::min(min_y, y); max_y = std::max(max_y, y);
  }
  lv_obj_set_style_translate_x(image, std::lround(-(min_x + max_x) / 2), 0);
  lv_obj_set_style_translate_y(image,
      std::lround(0.11547005f * h - (min_y + max_y) / 2), 0);
}
inline void turn(lv_obj_t *image, bool up) {
  if (!image) return;
  const int target = up ? 1800 : 0;
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
  // Preserve the configured accent or status color during motion.
}
inline void spin(lv_obj_t *image) {
  if (!image) return;
  lv_anim_delete(image, spindle_frame);
  lv_obj_update_layout(image);
  planar_frame(image, 0);
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
