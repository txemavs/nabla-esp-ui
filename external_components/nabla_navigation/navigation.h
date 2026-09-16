#pragma once
#include <string>
#include "forms.h"
#include "info.h"
namespace nabla {
struct Node { const char *title; int parent; const char *detail; const char *icon; int action; int bg_dark = 0; int bg_light = 0xFFFFFF; int icon_dark = 0xFFFFFF; int icon_light = 0; int info = 0; };
extern const Node nodes[];
extern const int count;
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
  return valid(n) ? (nodes[n].info ? nabla_info::value(nodes[n].info) : std::string(nodes[n].detail)) : "";
}
inline std::string row_title(int n) {
  if (!valid(n)) return "";
  return nodes[n].info ? std::string(nodes[n].title) + ": " + detail(n) : std::string(nodes[n].title);
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
