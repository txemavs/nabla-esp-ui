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

Camera rendering and live entity bindings remain planned. A generic camera widget
will accept a binding through its documented adapter contract; it must not embed a
specific installation's URL or entity. Do not introduce unimplemented YAML keys
to imply that this binding contract is already available.

## Device Builder with a pinned local library

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

A minimal private composition looks like this (supply the named secrets locally):

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
This is firmware configuration, not a completed runtime Wi-Fi editor: the current
on-screen Wi-Fi form is still the M2 simulation.

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
