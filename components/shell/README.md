# Reusable regular shell

package.yaml owns LVGL pages, toolbar/footer, form rendering and navigation
behavior. It deliberately does not import any example tree.

The consuming device YAML declares substitutions, hardware/theme/profile packages,
the device-info package, the local nabla_navigation external component source,
and nabla_navigation.tree/forms. That root is the editable device configuration
in ESPHome Builder. Reuse implementation; keep application composition local.

See simulator/composition.yaml for a full self-contained host composition.
examples/hello-world/ui.yaml remains a convenience wrapper around this shell and
its example catalog. Do not use that wrapper when the device must own its menu.

Initial appearance substitutions: nabla_initial_dark, nabla_initial_borders and
nabla_initial_font_choice (0 Ubuntu Mono, 1 DejaVu Sans). Restored user preferences
take precedence over initial defaults after the first boot.
