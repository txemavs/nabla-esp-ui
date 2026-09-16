# nabla_runtime

Build-time helper exporter used by reusable YAML modules. It accepts a list of
entries with a required headers list. Paths must belong to the allowlist in
__init__.py; arbitrary filesystem paths are rejected.

The exporter resolves headers relative to its own Git checkout and schedules
ESPHome's include_file operation at CoroPriority.FINAL, matching normal
esphome.includes ordering. It has no device state, controls or input events.
It adds no runtime component and selects only the helpers requested by packages.
The existing bounded C++ helpers own their runtime resource limits.

Example (normally supplied by the logo package):

    nabla_runtime:
      - headers:
          - components/logo/motion.h

Dependencies: ESPHome 2026.8.2 code generation; selected headers may require
LVGL or nabla_navigation. Both local and pinned Git sources have been compiled
on the host. See docs/GITHUB-LIBRARY.md for the public consumer entry point.
