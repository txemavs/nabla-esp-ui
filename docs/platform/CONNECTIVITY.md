# Wi-Fi commissioning and device cooperation

Status: design; no BLE peer or editable Wi-Fi workflow is implemented yet.

## Distinct adapters

Local ESPHome entities are the first adapter. Home Assistant uses its native
ESPHome integration where appropriate; MQTT and Nabla Edge are optional adapters.
None is a mandatory broker for local actions.

Bluetooth provisioning, remote control, telemetry subscriptions and HID are
different capabilities. A Home Assistant Bluetooth proxy is not automatically
a general peer-to-peer protocol. A BLE-capable chip is not necessarily capable
of Bluetooth Classic. Identify actual hardware before selecting transports.
See [primary sources](SOURCES.md).

## Local Wi-Fi flow

Settings > Wi-Fi shows radio state, current network, Scan and Add hidden network.
A scan is asynchronous and cancellable. Snapshot results have stable identity,
SSID display name, signal estimate, security and optional band/BSSID details.
Do not jump focus on every RSSI change. Group duplicate SSIDs carefully; preserve
security distinctions and allow details when networks share a name.

Selecting a result opens its form:
- Show the target device and SSID; a hidden network permits manual SSID entry.
- Open network needs no password; protected network uses the password editor.
- Show only authentication types supported by the adapter; do not classify
  every join error as 'wrong password'.
- Validate byte limits and encoding according to the target Wi-Fi stack;
  visual character counts alone are insufficient.
- Apply shows connecting, acquiring address and success/failure separately.
  Internet access is optional; a LAN-only join can succeed.

Credential changes are transactional at the application layer: retain the last
known-good configuration until the candidate is tested and persistence succeeds.
If the underlying ESPHome adapter cannot provide that behavior directly, mark
the feature experimental until the adapter implements it. Avoid persisting on
each keystroke. Cancellation stops uncommitted work; timeout returns a useful
error and retry option. A failed candidate must not strand the target with no
recovery path: preserve local commissioning or an explicit fallback mechanism.

Wi-Fi firmware credentials may use !secret. Runtime credentials belong to a
separate device-owned store, with bounded writes and platform-supported protection.
Factory reset and Forget network are explicit actions with clear consequences.
The UI must remain usable during scanning, failed association and radio reset.

## Large device as keyboard for a small one

Proposed user journey:
1. On the small target, enter Configure with another device; advertise briefly.
2. On the controller, open Nearby devices, select identity, and request Identify.
3. Confirm the intended target through its screen/button/code or another
   physically verified mechanism. A friendly name alone is insufficient.
4. Negotiate supported provisioning method and authorization.
5. Ask the target to scan if supported. Otherwise manual SSID entry or clearly
   labeled controller scan results are available; the two radios may see different APs.
6. Edit the credential on the larger screen, confirming the target before Apply.
7. Transfer once in the authorized session; target tests the network and reports
   accepted, connecting, connected and saved separately.
8. On success, close the session and discard controller-side secret buffers.
   If the link disappears, query operation status rather than resending blindly.

Evaluate Improv BLE first for interoperability. ESPHome provides the receiver;
a native large-panel client is additional work. The protocol has capabilities
and versioned commands; do not assume the pinned receiver implements every
command in the latest specification. Standard protocol compatibility does not
by itself prove confidentiality or authenticated pairing.
Physical authorization and transport encryption are separate checks.
If the chosen provisioning path cannot meet our credential-protection policy,
use a tested secure method rather than silently falling back to plaintext.
Keep a restricted compatibility mode, if needed, explicit and separately documented.

## Peer capability model

For ongoing cooperation, design a small versioned application protocol with:
device identity, protocol range, supported operations, value types, limits,
authentication requirements and optional form descriptions. Never accept code,
arbitrary executable YAML or raw memory/register access from a discovered peer.

Prefer a bounded common message schema over ad hoc per-widget characteristics.
Prototype transport-neutral messages in the simulator, then a BLE GATT transport.
CBOR is a candidate compact wire representation; JSON is suitable for fixtures
and diagnostics. Choose final encoding, UUIDs and framing after measurement,
not by documenting made-up production constants.

Envelope fields: protocol version, session ID, request ID, target capability,
message kind, sequence and payload length. Separate request acceptance from
result completion. Publish a maximum message size and reassembly timeout;
fragment according to negotiated MTU rather than assuming a large BLE packet.
Bound subscriptions, queues, payloads and concurrent requests.

Reliability: detect duplicate requests, sequence gaps and old sessions; idempotent
reads may retry, writes need an operation ID/deduplication rule. A reconnect
fetches current state and pending results. Expired commands never execute late.
A receiver validates types, ranges, permissions and capability versions before
dispatch; a checksum alone is not authentication.

## Trust and permissions

Provisioning permission does not imply remote-control permission.
Grant narrow capabilities for a bounded session; expose paired devices and
revoke/reset. Prefer authenticated encrypted transport using established stack
features. Headless targets without a trusted pairing channel require an explicit
provisioning design; do not claim that 'Just Works' guarantees peer identity.

Limit discovery windows and credential-transfer lifetime. Avoid broadcasting
network names or secrets unnecessarily. Redact sensitive fields by schema in
logs, traces and error reporting. BLE coexistence with Wi-Fi and display RAM
must be measured with the real board and simultaneous roles.

## Remote-control mode

The controller can either send typed domain actions (set brightness, next item)
or enter a negotiated remote-input session (UP/DOWN/LEFT/RIGHT/ENTER/ESC).
Prefer domain actions for version independence; input sessions are useful for
TV-like navigation. Both require allowlisted targets and explicit user context.

While controlling another device, show its name and an always-reachable Exit
remote mode action. Route commands to one selected peer only. Define arbitration:
local input at the target can revoke remote control; loss of heartbeat releases
held keys. Keyboard entry sends bounded editor content only into an authorized
form session, never into an arbitrary terminal.

Images/animations and video remain local assets or capability-gated streams;
the initial peer system is not remote desktop or arbitrary screen mirroring.

## Conformance scenarios

Wrong target; missing authorization; unsupported protocol/capability; oversized
or malformed payload; out-of-order fragments; duplicate Apply; lost result;
revoked peer; controller reboot; target power loss; cancelled form; no Wi-Fi;
connection succeeds but persistence fails; operation completes after controller
disconnects. Every case has a bounded failure path and preserves local navigation.
