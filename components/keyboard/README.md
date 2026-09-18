# On-screen keyboard

Latin keyboard maps for LVGL's native keyboard widget.

## latin_keyboard.h

Installs lowercase, uppercase, digits, ASCII punctuation and Spanish accents
(á, é, í, ó, ú, ü, ñ) into an LVGL keyboard. Retains LVGL editing, cursor,
Ready and Cancel behavior.

### Usage

```yaml
nabla_runtime:
  - headers:
      - components/keyboard/latin_keyboard.h

lvgl:
  widgets:
    - keyboard:
        id: my_keyboard
        ...
```

```cpp
#include "latin_keyboard.h"

// Install Spanish/Latin maps on a keyboard widget
nabla_keyboard::install(id(my_keyboard)->obj);
```

### Enabling in Wi-Fi forms

The Wi-Fi form package uses substitution `nabla_keyboard_latin`:

```yaml
substitutions:
  nabla_keyboard_latin: "true"   # Use Latin maps with accents
  # nabla_keyboard_latin: "false"  # Use stock LVGL keyboard
```

### Supported character repertoire

- Lowercase: a-z, ñ, accented vowels
- Uppercase: A-Z, Ñ, accented vowels
- Symbols: digits 0-9, standard ASCII punctuation
- Special keys: backspace, space, cursor left/right, OK, close

### Dependencies

- LVGL keyboard widget
- Shared text font with accent coverage (DejaVu or similar)

### Limitations

- Fixed QWERTY layout; no runtime localization
- No general IME or composed diacritics
- Spanish-focused; other Latin languages may need additional maps
