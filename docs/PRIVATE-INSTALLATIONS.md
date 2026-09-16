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

## Current packaging limitation

The working device entry point is devices/jc3248w535cn.yaml. Its transitive font,
image, C++ include and local external-component paths currently rely on the root
configuration living one directory below the repository root. YAML package
includes alone do not make all of these paths portable.

An arbitrary remote-package import into Device Builder is therefore not yet a
supported deployment recipe. Before publishing that recipe:

1. Make all transitive assets and external components resolve from an independent
   consumer configuration outside the checkout.
2. Validate and compile that independent consumer using a pinned revision.
3. Add the private device in Device Builder without copying installation data
   into the library.
4. Perform the first USB installation with networking and authenticated OTA.
5. Verify an actual subsequent OTA update, reboot and touch operation.

Document the tested import syntax only after this consumer build passes. The
current standalone firmware has no Wi-Fi/API/OTA; its simulated Wi-Fi editor does
not configure the real radio.

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
