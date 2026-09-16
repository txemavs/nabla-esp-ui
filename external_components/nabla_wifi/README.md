# Optional primary Wi-Fi adapter

Enable nabla_wifi: in a private ESP32/LVGL composition with wifi.networks and
nabla_navigation. The existing local external-component source discovers it.
The host and compact examples retain their deterministic mock backend.

The adapter copies 1..8 declared Wi-Fi networks (including their ESPHome options)
as immutable fallbacks; their priorities must be <=110. A manually saved primary
uses the highest configured priority plus 10. Network priority remains ESPHome's
selection policy, not a promise of instantaneous roaming to a newly visible AP.

## User flow

Settings > Connections > Wi-Fi edits an SSID/password or selects a scan result.
Open networks are supported; protected passwords require 8..63 UTF-8 bytes.
Connect tests the candidate alone for up to 30 seconds. The UI remains responsive
and Cancel/Back restores the previous saved primary plus all declared fallbacks.
Only the matching connected SSID held for two seconds qualifies for saving.
The editor erases its password draft after handing it to the adapter.

After saving, the adapter reinstalls the primary/fallback list and reconnects.
The success message means the primary was tested and saved; a brief reconnection
can follow. Failure restores the prior list without committing the candidate.
Repeated Connect while a request is in progress is ignored.

Search opens real scan / use fallback networks / back. Removing the primary
requires a confirmation whose default is Cancel. It clears the saved primary,
reinstalls the declared fallback networks and reconnects.

## Implementation and limits

The radio backend is separate from WifiFlow and the LVGL renderer. It copies at
most eight unique SSID/security pairs and retains stronger results on overflow.
Scanning times out after 15 seconds; stale callbacks cannot complete a connection
request. UI updates stay in the main loop and refresh without blocking delays.

A private ESPHome preference stores one bounded primary record (SSID/password).
Configured fallback credentials stay in the private YAML/secrets. No credentials
are added to public sources or adapter logs. Flash/NVS is not encrypted by this
component; use the platform's storage/security policy. Do not publish firmware,
flash backups or resolved configurations.

A storage error triggers best-effort restoration of the previous preference and
the known network list. Physical power loss during flash writes and unrecoverable
storage failures still need hardware fault testing. The independent preference
key remains stable across firmware updates.

Current hardware target: JC3248W535CN with ESPHome 2026.8.2. API/OTA sessions may
disconnect during a trial, reset or reconnection; local touch/Back remains usable.
The Builder's routed upload IP may need updating after changing networks.
The form does not configure Enterprise Wi-Fi, arbitrary byte SSIDs or 64-hex PSKs.
Compact physical rendering and broader board coverage remain future validation.

Host tests use the actual adapter against fake radio/preferences under sanitizers:
duplicate submission, cancel, timeout, wrong SSID, persistence/reload, three
fallback identities, failed save/sync, scan deduplication, late results and timer
rollover. These tests are not a claim of physical 24-hour soak coverage.

## Initial hardware evidence (2026-09-16)

A private bounded integration probe on the JC3248W535CN used the adapter's actual
flow and radio. It completed real scan, rejected an intentionally wrong password,
recovered a configured fallback, accepted/saved a valid primary, removed it and
reconnected through the fallback list. The probe reported PASS over USB.
The temporary probe is removed from the production firmware; final state is the
original fallback list with no test primary. No private credentials or scan
identities are part of this repository. Physical cold-power-cycle and long soak
coverage remain M3 gates rather than claims from this short probe.
