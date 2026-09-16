# Public library and private installations

The public repository contains reusable UI behavior, hardware adapters, schemas,
fonts, icons, tests and synthetic examples. A private installation composes these
pieces and supplies its own networks, entity bindings and credentials.

Home Assistant and ESPHome Device Builder are optional deployment tools. The UI
must remain usable locally while Wi-Fi, the router, a VPN or Home Assistant is
unavailable.

## Ownership boundary

- Public: camera controls and their states, light controls, navigation, inputs,
  themes, generic transport adapters and documented extension points.
- Private: actual camera entities, stream URLs, network names and addresses,
  device inventory, Home Assistant endpoints, MQTT topics and site-specific menus.
- Secret: Wi-Fi and OTA passwords, API encryption keys, broker credentials,
  tokens and any URL containing credentials.

Entity identifiers and addresses can disclose an installation even when they are
not passwords. Keep them out of public fixtures, screenshots, logs and commits.
Use invented entities and networks in examples.

Private device YAML belongs in the installation's ESPHome configuration directory
or a separate private repository, outside this checkout. Store credentials in
that installation's secrets.yaml and reference them with !secret. Ignoring a file
is not a substitute for this separation; never commit private firmware binaries,
backups or generated configurations.

## Composition and updates

The intended deployment entry point is a small private root YAML that selects:

1. A tested public library revision and hardware/profile composition.
2. Its name, language, navigation and optional application modules.
3. Private bindings for those applications.
4. Optional Wi-Fi, encrypted native API, OTA and time adapters.

Pin the public revision so updating one installation is deliberate and repeatable.
Develop reusable behavior in this repository and validate it before advancing the
private installation's pin. Updating a public module must not publish the private
configuration or update unrelated devices.

The experimental [camera viewer](../components/camera-viewer/README.md) accepts private HA camera bindings. RTSP decoding and compact camera rendering remain unimplemented.

## Device Builder with a GitHub library

Use [the remote library entry point](GITHUB-LIBRARY.md) for new installations.
It downloads reusable packages, helpers and assets at the same pinned revision.
Keep the device-owned menu below in the root YAML and preserve all private
network/API/OTA blocks when replacing local imports.

## Optional local checkout for development

The supported consumer layout keeps one public checkout next to a private root
configuration. Set nabla_resource_root to that checkout; fonts, images, C++ headers
and the local external component use this root. Its default is .. so the existing
device and simulator entry points retain their layout.

Example directory structure:

    esphome/
      .nabla-ui/              # Public library checkout at a tested commit
      panel.yaml              # Private device configuration
      secrets.yaml            # Private credentials, managed by the installation

The public checkout can be inspected in the installation's file editor. Device
Builder's device entry opens the private composition and can build its includes.
Do not edit generated C++ or duplicate UI behavior into the private root.

The base device import below is a bring-up shortcut, not the recommended editable
menu composition. For a device-owned menu use the structure described under
Device-owned menus below.

A minimal bring-up composition looks like this (supply the named secrets locally):

```yaml
substitutions:
  nabla_resource_root: .nabla-ui
  ui_language: en

packages:
  base: !include .nabla-ui/devices/jc3248w535cn.yaml

wifi:
  reboot_timeout: 0s
  networks:
    - ssid: !secret panel_wifi_ssid
      password: !secret panel_wifi_password
      priority: 10

ota:
  - platform: esphome
    password: !secret panel_ota_password
```

Clone the public repository into .nabla-ui and check out an explicitly selected,
tested commit before validating the private root. For upgrades, check that the
library checkout is clean, fetch, and select the new tested commit; then validate
and install only the intended device. Do not automatically update all devices.

An optional manually supplied network can precede fallback networks with a higher
priority (for example 40 above fallbacks at 30/20/10). Simply moving an entry to
the top does not replace explicit priority. Keep its SSID/password in local
secrets. An omitted manual entry leaves the fallback configuration unchanged.
This commented network block is firmware configuration. To enable the runtime
primary editor on an ESP32/LVGL composition, add nabla_wifi: at the private root.
The adapter derives its fallbacks from wifi.networks; see its README for limits.
Without this opt-in the on-screen form remains the M2 simulation.

An arbitrary remote YAML package import is not supported by this recipe: the
resources must exist in the local checkout selected by nabla_resource_root.
Direct GitHub-only asset packaging remains a separate improvement.

Validation includes a hardware firmware build from a private consumer directory
outside this repository without sibling asset symlinks, and configuration
validation of the regular, compact and password simulator entry points.
The standalone public device YAML remains offline; Wi-Fi/OTA are opt-in additions
in the private root. The first network-enabled USB installation and a subsequent
authenticated OTA upload have been verified for the initial board.

## Routed OTA and offline operation

A routed VPN can make devices on another site's network reachable from the build
server. It does not merge Wi-Fi radio networks, and multicast discovery must not
be assumed to cross routed links. Use a privately configured reachable upload
address when discovery does not work.

The server performing the upload needs a route to the device and access to its
OTA service. A public Home Assistant browser URL alone does not establish this
path. Retain OTA authentication even when traffic crosses a private VPN.

For a mobile panel, verify that Wi-Fi/API connection loss does not trigger a
reboot loop or block local navigation. Model radio association, router service,
VPN reachability and Home Assistant availability separately. Local functions must
not wait for an unrelated remote service.

## Verification boundary

An online entry in Device Builder is evidence of its reported availability, not
proof that a new firmware was compiled or uploaded. Distinguish configuration
validation, successful compilation, verified USB writing, observed physical
behavior and a completed OTA test in reports.

## Device-owned menus

The Builder device YAML is the source of truth for the device model, display
capabilities, initial appearance, application list, navigation tree, forms and
installation bindings. Import components/shell/package.yaml, hardware, theme and
profile independently. Declare external_components with the local checkout path
and nabla_navigation.tree/forms directly in the root; include device-info/package.yaml
for information bindings. Do not import examples/hello-world/ui.yaml or the
standalone devices demo in this composition: they bring an example menu.

simulator/composition.yaml is a complete public, credential-free reference.
Translate its local paths to .nabla-ui in Builder and supply the hardware adapter
instead of SDL. Preserve existing wifi, api, ota and secrets references.
Changes in the Builder root belong to the installation; they must not be overwritten
by library updates. Advancing the library checkout does not replace the root menu.
User runtime preferences override initial defaults when already persisted.
