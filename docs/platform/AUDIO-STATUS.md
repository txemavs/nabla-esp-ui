# Bluetooth audio handoff — 2026-09-20

Status: paused experiment, not production audio. Keep off main pending review.

## Verified evidence

Original ESP32 T-Call with compact OLED and encoder detected a Galaxy Buds Pro
headset. Authentication succeeded and HFP state 3 (SLC_CONNECTED) was logged.
The owner confirmed the connected display. This does not establish audio.

Previous builds: audio opening reached the connecting state, then failed or
timed out. The owner heard no tone. No successful audio-open state, microphone
input, Assist interaction, keyboard/audio coexistence or reliable reconnect is
verified.

## Root cause identified (2026-09-20)

CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN was unset, defaulting to 0. This disabled
SCO/eSCO connections at the controller level. The HCI Setup Synchronous
Connection command requires at least one sync connection slot.

**Fix:** Set CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN=1 in the component's
sdkconfig options. This matches the ESP-IDF HFP AG example configuration.

Ready for owner retest with Kit1 or T-Call + headset. Expected behavior:
1. After HFP SLC connects (state 3), press Test Tone
2. Audio state should transition: 1 (CONNECTING) → 2 (CONNECTED) or 3 (CONNECTED_MSBC)
3. Log shows sync_conn_handle and frame_size
4. 440 Hz tone plays for 0.5 seconds
5. Audio disconnects, status shows "Tono enviado"

## Earlier findings

Disabling compiled HFP client support caused "Out of Service Records (3)";
restoring it recovered AG service connection. Only AG is initialized.
Host/controller HCI selection and a CVSD-only baseline are in place.
Unknown-codec and 3-EDR eSCO capability warnings are observations from the
Bluedroid stack, not proven causes of the timeout.

## Resume checklist

1. **Physical retest required:** Flash updated firmware, connect headset,
   trigger Test Tone after SLC connects. Verify audible 440 Hz tone.
2. If timeout persists, capture full bt_audio logs including Audio event details.
3. Add timeout, cancellation, lost-link and tone-limit regression tests.
4. Refactor shared Bluetooth ownership before combining HID keyboard and HFP.
5. Complete localized search/confirm/cancel/disconnect/forget menus.
6. Integrate Assist only after headset audio is functional.

The host session-helper tests validate bounded buffering and cancellation, not
radio interoperability. Preserve each installation's private configuration and
recovery firmware outside this repository before any later deployment.
