// Printable common Latin keys. Unsupported/dead keys emit no guessed character.
#pragma once
#include <cstdint>
namespace esphome::nabla_bt_keyboard {
inline char latin_key(uint8_t key,uint8_t modifiers) {
  if(modifiers & 0xDD)return 0; // Ctrl/Alt/GUI shortcuts are not printable.
  bool shift=modifiers & 0x22;
  if(key>=4 && key<=29)return (shift?'A':'a')+key-4;
  if(key>=30 && key<=39)return (shift?"!@#$%^&*()":"1234567890")[key-30];
  if(key==44)return ' ';
  return 0;
}
}
