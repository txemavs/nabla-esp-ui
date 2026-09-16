# Shared navigation

The current static catalog is in tree.h; translations are applied from
translations.yaml using locales/es.yaml or locales/en.yaml. Declarative
navigation YAML remains planned; see docs/ARCHITECTURE.md.

logic.yaml supplies open, back, home, move, activate, choose and render actions.
Touch and keyboard use the same activation path with 120 ms feedback.
Focus and content scroll position are saved per node and restored on return.

Applications use components/toolbar.yaml:
- Left triangle: return to the desktop.
- Breadcrumbs: open an ancestor or stay at the current node.
- Right X: close to the parent; at application root this returns to desktop.
- Paths are measured using the actual font. When space is insufficient, earlier
  ancestors become a clickable ... link to the immediate parent.
- The current title is retained and truncated with dots only if needed.
- Toolbar is hidden on the splash and desktop.

Sequential focus order is content items, Home, visible breadcrumbs, X.
Up/Down wraps, Enter activates, Escape returns, Home opens the desktop.
Left/Right remains reserved. All displayed toolbar actions are touchable and
reachable without a physical Escape key.

The initial profile is 480 x 320. Breadcrumbs have up to three slots.
Lists support eight children and scroll. Root tiles still duplicate catalog
titles through translation keys; a unified YAML catalog is the next milestone.
Settings pages are informational placeholders; sensors are not connected.

Validation:
```sh
g++ -std=c++17 tests/navigation.cpp -o /tmp/nabla-navigation-test
/tmp/nabla-navigation-test
```
