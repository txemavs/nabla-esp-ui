# Consume the library from GitHub

The device owns its navigation tree, forms, hardware selection and private
bindings. GitHub supplies reusable presentation, controllers, translations,
fonts and assets. No manually maintained .nabla-ui copy is required.

## Repository contract

- packages/regular.yaml: public LVGL entry point; no imported example menu.
- external_components/: ESPHome schemas and code generation.
- external_components/nabla_runtime/: exports allowlisted C++ helpers from the
  same Git checkout as the component, using ESPHome's late include generation.
- components/, navigation/, theme/, locales/, profiles/: reusable packages.
- hardware/: optional board adapters.
- examples/: complete device-owned compositions, with synthetic data only.
- simulator/ and tests/: development fixtures and checks.

The existing components/ directory contains YAML widgets. Always specify
source.path: external_components for Git external components; do not rely on
ESPHome's default components/ discovery.

## Versioning

Set nabla_ui_ref to an immutable full commit SHA. Use that same substitution
for the package ref. The entry point uses it for external components and raw
font/logo URLs too. Do not mix versions. A moving main branch is useful during
development but is not a reproducible installation.

ESPHome downloads and caches dependencies at build time; compiled devices do
not fetch their UI from GitHub at startup. Subsequent operation does not require
GitHub or Home Assistant. A fresh build does require network access.

## Consumer structure

See examples/github/panel.yaml for the complete editable starting point.
Copy that YAML into Device Builder, select a tested revision, then configure
your device's Wi-Fi, API and OTA privately with !secret references.

Keep the full nabla_navigation tree and forms in that root YAML. Importing a
library must not hide the installation's menu inside an example package.
Hardware is imported separately, so the same library can serve other boards.
The regular entry point currently targets LVGL displays; compact OLED packages
remain available through the local development workflow.

Updates are deliberate: change one revision, validate, compile, then install
only the intended device. Changing the library pin alone does not flash anything.
Keep the previous revision for rollback. The project is pre-1.0; schema changes
must be documented before introducing release tags.

## Development

Existing local simulator entry points continue to use local external components.
The runtime exporter replaces only helper path resolution; source headers stay
in their module directories and are not duplicated in the repository.
Fonts and artwork retain their bundled licenses.

Validate a remote consumer from a separate directory with no sibling checkout.
A successful build there catches accidental dependencies on ../ or .nabla-ui.

## Verification

Revision 4acd4ecdca042eedbaa3824ca5492abf805fd4aa passed a clean GitHub-only host build on
ESPHome 2026.8.2, from a separate consumer directory. Local composition, compact
and password builds and all 12 unit tests also passed. No firmware was uploaded.
