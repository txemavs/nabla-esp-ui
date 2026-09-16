#pragma once
#include <array>
#include <string>
namespace nabla_info {
inline std::array<std::string, 6> values{};
inline std::array<std::string, 6> details{};
inline std::string detail(int field) {
  return field > 0 && field < 6 ? (details[field].empty() ? values[field] : details[field]) : "";
}
inline std::string value(int field) { return field > 0 && field < 6 ? values[field] : ""; }
}
