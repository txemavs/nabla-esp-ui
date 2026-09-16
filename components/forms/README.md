# Local Wi-Fi workflow and compact text draft

Status: M2 Wi-Fi vertical slice; deterministic simulation, no radio or persistence.
wifi_flow.h is independent of LVGL, display drivers, Home Assistant and GPIOs.

WifiFlow owns bounded validation and these states:
editing, scanning, results, connecting, success, failure.
Scans finish after 700 ms; connections finish after 1000 ms when tick(now) runs.
Cancellation changes state immediately, so a later tick cannot finish an old
operation. A second submit during an operation is ignored.
A new operation increments revision; this is a local simulator counter, not a
network protocol or server-side deduplication implementation.

Fixtures:
- Nabla Demo: protected, simulated success.
- Invitados Demo: open, no password required.
- Demo sin servicio: protected, simulated failure.
- Café Demo: protected, accented SSID, simulated success.
- Empty scan: available in the compact Wi-Fi menu and the shared model.

A submitted protected network requires 1..32 SSID bytes and 8..63 password bytes.
Open networks require only the SSID. Every connection discards the password
from the workflow immediately; failure/retry requires re-entry on protected
networks. No simulated success promises real authentication or connectivity.
No actual credentials are published, persisted or logged.

TextDraft provides bounded UTF-8 append and deletion of the last code point.
CompactWifi adds field-level accept/cancel and a selector of lowercase,
uppercase, digits and punctuation including space. Its Latin character set
includes Spanish accents; no Unicode normalization or general IME is promised.
The shared primitives are independent of presentation. Large displays use the
existing LVGL keyboard; tiny/readable profiles use the character selector.

## Compact controls

Wi-Fi opens a scrollable form: SSID, Password, Open network, Scan, Empty scan,
Connect, Back. Up/Down moves and Enter activates. Field editing begins on Done;
the following controls are Delete, Cancel and palette switch, then characters.
The draft changes only on Done; Escape/Cancel discards that field edit.
Home discards the entire workflow. Password previews are always masked.
On touch, select a form row; in the picker tap the center to activate and the
left/right footer halves to move. Tapping the header goes back.

## Regular controls

Use Settings > Connections > Wi-Fi. Scan opens a local list over the form;
selecting a result fills the SSID and open/protected flag. The open checkbox
can also be changed manually. Up/Down/Enter traverses fields, keys, Connect,
Scan, the checkbox and Cancel. During connection, Cancel stays available.
Results remain explicitly marked demo. Editing after a result clears its status.

## Scope remaining in M2

This delivers one complete local Wi-Fi test path, not the entire forms catalog.
Generic numeric editors, reusable dialogs, duplicate-network selection policy,
scan-error fixtures, enterprise/raw-PSK authentication, full Unicode editing,
and production persistence/rollback remain pending. Physical networking is M3.
