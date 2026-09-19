# T-Call OLED preview

Run the complete host root with ./simulator/run.sh tcall. It uses the tiny 128x64 three-row layout with a 12 px body font. navigation.yaml is a menu fragment,
not a standalone firmware. The public site label is generic; tcall_site_title
can be overridden in a private composition.

The preview has local on/off light states, an unavailable light, simulated Wi-Fi,
appearance and device information. It never connects to MQTT or controls real
entities. Information describes the host preview; Wi-Fi/IP remain unavailable
until a real device backend is supplied.

Normal mode uses a triangle pointer throughout menus, Wi-Fi and generic forms.
Disabling Normal mode selects inverse highlighting. Dark/light and font choices
remain shared. Up/Down moves, Enter activates, Escape cancels/returns and Home
returns to the root. The root triangle switches list/single-icon presentation.

Hardware is intentionally not flashed by this preview. The historical private
T-Call YAML uses a separate renderer; reconcile the deployed configuration before
migrating its MQTT/BLE/network bindings. Keep those bindings and credentials
outside the public repository.

## Password entry

The bottom character strip is operated with sequential input: turn to select,
press to append. Before the first letter are Done, Delete, Cancel and a palette
switch (abc, ABC, digits, symbols). The logo is an additional reachable Back
target. Cancel discards the field draft; Done accepts the field without connecting.
Passwords remain masked, including the last typed character.

compact_wifi_clean_ui hides diagnostic scan scenarios and demo labels while
retaining the host mock backend. It does not enable a real radio. Device builds
need nabla_wifi_compact and their private network configuration. Choosing a
protected scan result opens password entry.
