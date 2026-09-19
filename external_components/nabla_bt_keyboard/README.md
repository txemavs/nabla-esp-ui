# Classic Bluetooth keyboard (experimental)

Optional ESP-IDF Bluedroid HID host for the original ESP32 only. S3/C3 and BLE
keyboards are not supported by this adapter. It cannot coexist with ESPHome's
BLE stack in this first version.

The T-Watch example adds Settings > Connections > Bluetooth > Keyboards.
Search collects up to four keyboard-class devices. Choose one explicitly.
PIN shows the code to type on the keyboard followed by Enter; numeric-comparison
pairing requires selecting PIN to confirm after comparing. Pairing times out
after 60 seconds. Disconnect stops automatic reconnection. Forget removes the
saved peer and requests removal of its Bluetooth bond. No pairing keys enter YAML.

Successful peers are saved locally. On restart or link loss, reconnection is
attempted at most three times; Search remains the manual recovery path.
The first version requires HID boot keyboard reports (8 bytes), with press-edge
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
