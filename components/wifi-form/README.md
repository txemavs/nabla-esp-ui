# Manual Wi-Fi form

The package in package.yaml opens at Settings > Connections > Wi-Fi through
action: wifi_demo. It is a host-only simulation on regular/portrait LVGL.
The compact renderer uses the same WifiFlow with its character selector.

The form provides SSID, masked password, an open-network checkbox, a native
keyboard and Connect/Scan/Cancel. Scan opens normal/empty/error fixtures; results
fill the draft. Nothing changes the host's Wi-Fi or stores credentials.

Up/Down/Enter reaches fields, keyboard keys and all actions. Touch updates the
same focus. Connection starts at most once per submitted draft; Cancel remains
available while busy. Invalid input focuses the field requiring correction.
Leaving through the toolbar, Home or Cancel clears drafts.

The optional Latin keyboard supports ASCII and Spanish accents. The last
password character is never briefly revealed. Portrait places fields vertically;
landscape places them side by side, above the keyboard.

See [shared forms](../forms/README.md) for validation, fixtures, resource bounds,
extension rules and test evidence. Real Wi-Fi and Nabla Net readiness are M3+.
