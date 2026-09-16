#pragma once
#include <array>
#include <string>
namespace nabla_info {
inline std::array<std::string, 6> values{};
inline std::string value(int field) { return field > 0 && field < 6 ? values[field] : ""; }
}
