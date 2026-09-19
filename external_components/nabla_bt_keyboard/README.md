# Classic Bluetooth keyboard (experimental)

Optional ESP-IDF Bluedroid HID host for the original ESP32 only. S3/C3 and BLE
keyboards are not supported by this adapter. It cannot coexist with ESPHome's
BLE stack in this first version.

The T-Watch example adds Settings > Connections > Bluetooth > Keyboards.
Search collects up to four keyboard-class devices. Choose one explicitly.
The first menu row, PIN, shows the code to type on the keyboard followed by Enter; numeric-comparison
pairing requires selecting PIN to confirm after comparing. Pairing times out
after 60 seconds. Disconnect stops automatic reconnection. Forget removes the
saved peer and requests removal of its Bluetooth bond. No pairing keys enter YAML.

Successful peers are saved locally. On restart or link loss, reconnection is
attempted at most three times; Search remains the manual recovery path.
Supports HID boot keyboard reports (8 bytes) and descriptor-validated byte-aligned
keyboard-array reports (IDs 1–15, up to 32 payload bytes), with press-edge
events and no typematic repeat. Rollover reports do not generate characters.
Only the selected peer can deliver input. Queue overflow disconnects the session.

on_key emits HID usage and modifiers on the main loop; no display dependency is
introduced in the transport. The example maps Up/Down/Enter/Escape/Home into
existing scripts. The Wi-Fi text field accepts letters, digits, space and
backspace; shifted digits currently use US symbols. Full Spanish layout, dead
keys and general form text input remain pending. Do not claim full text support.

Bluetooth callbacks copy events into a fixed 24-entry queue. No typed input is
logged. Discovery is user-initiated and ends after a bounded inquiry.
Bluetooth adds runtime memory requirements; physical coexistence with Wi-Fi,
display and OTA must be measured before claiming support.

Reference: Espressif ESP-IDF examples/bluetooth/esp_hid_host and the pinned
ESP-IDF 5.5.5 esp_gap_bt_api.h / esp_hidh_api.h APIs.

## Physical evidence (2026-09-19)

The owner confirmed pairing and menu navigation on a T-Watch 2020 with a
Bluetooth 3.0 Keyboard after USB deployment. Escape behavior and text entry
remain unverified. This is one tested keyboard, not general HID qualification.

The keyboard accepts boot negotiation but sends nine-byte packets retaining
the report ID. The decoder accepts these only when the report descriptor,
ID and exact length match; malformed or unknown reports remain rejected.
Host regression tests cover this case and standard eight-byte boot packets.
All 26 repository tests pass and the ESPHome target builds successfully.

Reconnection also produced authentication failures during testing; forgetting
the bond and pairing again recovered the connection. Reliable reconnection
across restarts and Wi-Fi coexistence still need qualification.

OPEN/CONNECTING is an acknowledgement, not a completed HID connection:
protocol negotiation must wait for OPEN/CONNECTED. Diagnostics log only
transport metadata, never key contents or PINs.
