#pragma once
#include <string>
namespace nabla {
struct Node { const char *title; int parent; const char *detail; };
static Node nodes[] = {
  {"Inicio", -1, ""},
  {"Ajustes", 0, ""}, {"Reloj", 0, "Aplicación pendiente"},
  {"Fotos", 0, "Aplicación pendiente"}, {"Música", 0, "Aplicación pendiente"},
  {"Vídeo", 0, "Aplicación pendiente"}, {"Tiempo", 0, "Aplicación pendiente"},
  {"Luces", 0, "Aplicación pendiente"}, {"Sensores", 0, ""},
  {"Wi-Fi", 1, "Simulador: red del ordenador.\nLa configuración Wi-Fi del ESP está pendiente."},
  {"Idioma", 1, "Idioma actual: Español.\nEl cambio de idioma está pendiente."},
  {"Acerca de", 1, "nabla.net ESP UI\nESPHome + LVGL\nSimulador de escritorio"},
  {"Salón", 8, ""}, {"Exterior", 8, ""},
  {"Temperatura", 12, "Sin sensor conectado"}, {"Humedad", 12, "Sin sensor conectado"},
  {"Temperatura", 13, "Sin sensor conectado"}
};
constexpr int count = sizeof(nodes) / sizeof(nodes[0]);
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
