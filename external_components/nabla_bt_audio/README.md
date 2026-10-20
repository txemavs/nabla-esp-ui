# Bluetooth audio — experimental, PAUSED

Optional HFP Audio Gateway for original ESP32 with ESP-IDF.
**Status (2026-09-20): paused.** HFP SLC works; SCO audio not achieved.

Discovery, explicit candidate selection, SSP confirmation and HFP service
connection are implemented. A bounded, manually triggered tone attempt exists.
No successful audible playback, microphone capture or Assist session is verified.

The prototype owns Bluedroid and conflicts with nabla_bt_keyboard and ESPHome
BLE components. Shared radio ownership, reconnect handling, localization and
complete AT service handling remain pending. Legacy PIN pairing is rejected.

Configuration: nabla_bt_audio with id and optional peer (Bluetooth MAC).
Keep peer bindings in private device YAML.
Methods: scan, candidate_count, candidate, available, select, needs_confirmation,
confirm, connect, disconnect, connected, status and test_audio.
Call these from the main loop. connected means HFP service, not audio.

Discovery is bounded to eight candidates. Callback discovery events use a
bounded queue. Audio input is discarded; no microphone data is logged.
The tone uses legacy PCM callbacks, a periodic timer and a low-level 440 Hz
signal limited to half a second. Audio opening times out after ten seconds.
Status distinguishes timeout, SCO rejection, and completion.

The controller is configured with one synchronous connection slot
(CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN=1), required for SCO/eSCO.
Host and controller select HCI for the SCO data path. Wideband speech is
enabled (CONFIG_BT_HFP_WBS_ENABLE=true) for codec negotiation. HFP client
support remains compiled as a service-record-capacity workaround.

Current status: SCO connection is actively rejected by Galaxy Buds Pro after
~3 seconds ("SCO rechazado"). Four fix attempts on 2026-09-20 did not achieve
audio. See handoff document for investigation history and resume steps.

See [handoff](../../docs/platform/AUDIO-STATUS.md) and the
[compile fixture](../../examples/bluetooth-audio/bringup.yaml).
