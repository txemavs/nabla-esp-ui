# Device-owned commands

A command is a non-root leaf with action: command. Register it once in
nabla_navigation.commands using node: <key>, available: !lambda, state: !lambda
and on_press: <native ESPHome automation>. Missing, duplicate and orphan
registrations fail validation. Device bindings remain in the consumer YAML.

Both LVGL and compact navigation dispatch the same command and remain in the
parent menu. The regular list refreshes its displayed state once per second.
Touch and U/D/ENTER use the same activation path. Availability is rechecked at
activation; disconnected or unknown states must return false. State lambdas
must report authoritative data, never assume an action succeeded. Labels are
limited to 48 bytes (keep UTF-8 labels short). Lambdas must be fast/nonblocking.

For Home Assistant use an internal homeassistant text_sensor for the entity
state. Enable availability only for an active API state subscription and an
on/off state. Send explicit turn_on or turn_off, using the current state, and
wait for the imported state to change. Do not queue offline actions. The HA
ESPHome integration must allow this device to perform Home Assistant actions;
having it in Device Builder alone is insufficient.

The dispatch table is bounded to the existing 128-node tree limit. It currently
allocates three std::function slots per node, including unused nodes. Per-command
automation resource cost depends on the consumer. Optimize storage before
targeting very small MCUs. No Home Assistant dependency is added to the core.

Verified: ESP32-S3 compile with a private HA switch control; unit coverage blocks
offline/out-of-range/unregistered execution. Physical switching is not yet
verified. This is not a camera renderer or a generic Lovelace importer.
