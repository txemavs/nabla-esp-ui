#pragma once
#include <string>
#include <functional>
#include <array>
#include "forms.h"
#include "info.h"
namespace nabla {
struct Node { const char *title; int parent; const char *detail; const char *icon; int action; int bg_dark = 0; int bg_light = 0xFFFFFF; int icon_dark = 0xFFFFFF; int icon_light = 0; int info = 0; };
extern const Node nodes[];
extern const int count;
struct Command {
  std::function<bool()> available;
  std::function<std::string()> state;
  std::function<void()> execute;
};
inline std::array<Command, 128> commands;
inline void execute_command(int n) {
  if (n >= 0 && n < 128 && commands[n].available &&
      commands[n].available() && commands[n].execute) commands[n].execute();
}

inline bool valid(int n) { return n >= 0 && n < count; }
inline int child(int parent, int index) {
  for (int n = 1; n < count; n++) if (nodes[n].parent == parent && index-- == 0) return n;
  return -1;
}
inline int children(int n) {
  int total = 0;
  while (child(n, total) >= 0) total++;
  return total;
}
inline std::string detail(int n) {
  return valid(n) ? (nodes[n].info ? nabla_info::detail(nodes[n].info) : std::string(nodes[n].detail)) : "";
}
inline std::string row_title(int n, bool dark = true, int family = 0, bool borders = true) {
  if (!valid(n)) return "";
  if (nodes[n].action == 8) return std::string(nodes[n].title) + ": " +
    (commands[n].state ? commands[n].state().substr(0, 48) : "--");
  if (nodes[n].action == 5) return std::string(dark ? "[x] " : "[ ] ") + nodes[n].title;
  if (nodes[n].action == 7) return std::string(borders ? "[x] " : "[ ] ") + nodes[n].title;
  if (nodes[n].action == 6) return std::string(nodes[n].title) + ": " + (family == 0 ? "Ubuntu Mono" : "DejaVu Sans");
  return nodes[n].info ? std::string(nodes[n].title) + ": " + nabla_info::value(nodes[n].info) : std::string(nodes[n].title);
}
inline std::string path(int n) {
  if (!valid(n)) return "";
  std::string result = nodes[n].title;
  for (int depth = 0; nodes[n].parent >= 0 && depth < count; depth++) {
    n = nodes[n].parent;
    result = std::string(nodes[n].title) + " > " + result;
  }
  return result;
}
}
