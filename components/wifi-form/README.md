# Manual Wi-Fi form (simulator)

Status: integrated local demo; no radio, persistence or automatic discovery.
Include package.yaml alongside the navigation shell and select action: wifi_demo
on a non-root leaf. The package declares its widgets/scripts and depends on
nav_back, shared fonts, locale substitutions and the shell's runtime dimensions.
Only one active editor instance is supported.

The C++ Editor helper owns field selection, sequential input, bounded key
traversal and regular/portrait layout. Menu routing stays in navigation/logic.yaml.
The form returns through the ordinary breadcrumb, triangle, X and Escape paths.
Leaving the form clears both fields; changing fields does not clear the draft.

Inputs:
- Touch a field to attach the native keyboard to it.
- Up/Down traverses SSID, password, keyboard keys, Apply and Cancel.
- Enter on a field enters keyboard selection; Enter on a key types it.
- The native Ready key advances SSID to password, then submits.
- Cancel on the keyboard/button or Escape returns without committing.
- Home returns to the launcher and clears the draft.

States: editing, invalid SSID, invalid password and demo accepted.
Validation requires SSID length 1..32 UTF-8 bytes and password length 8..63 bytes.
The open-network checkbox bypasses password validation; scans select its state.
64-digit raw PSKs and enterprise authentication are not implemented.
The textarea's character limit is only an editing limit; submission checks bytes.
The stock keyboard has its native repertoire, not a Spanish custom key layout.

Apply validates locally, clears the password and runs the shared simulated
connection operation before reporting demo success/failure.
No connection is attempted, no credentials are saved, and no input is logged.
On real hardware the future Wi-Fi adapter will implement connection state,
failure, rollback and deliberate persistence. Nabla Net automatic discovery is
a separate planned adapter using the same application state.

Supported preview: 480x320 and rotated 320x480, es/en labels, dark/light shell.
Field/key surfaces follow the same black/white theme as the shell. Tiny OLED editing is
pending; this composition is not automatically scaled down to 128x64.

M2 adds a Scan overlay and the shared [Wi-Fi workflow](../forms/README.md).
Connection and scan waits are deterministic and cancelable; no network changes.
