# Bluetooth audio handoff — 2026-09-20

Status: paused experiment, not production audio. Keep off main pending review.

## Verified evidence

Original ESP32 T-Call with compact OLED and encoder detected a Galaxy Buds Pro
headset. Authentication succeeded and HFP state 3 (SLC_CONNECTED) was logged.
The owner confirmed the connected display. This does not establish audio.

## Investigation history

### Fix #1: CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN (commit 1422bbf)

CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN was unset (default 0), disabling
SCO/eSCO at the controller level. Fixed by setting to 1.

**Result:** Physical retest on Kit1 + Galaxy Buds (2026-09-20 ~18:58 CEST)
still showed 10-second timeout. Audio state went to CONNECTING (1) but never
reached CONNECTED (2). SLC then disconnected. MAX_SYNC_CONN=1 necessary but
not sufficient.

### Fix #2: Enhanced diagnostics and missing AT handlers (current)

Added comprehensive logging and missing HFP AT command handlers:
- All HFP callback events logged with names
- Connection state logs peer_feat and chld_feat (shows headset capabilities)
- BCS codec negotiation event logged
- CLCC (call list) and CNUM (subscriber number) responses added
- Audio state transitions logged with state names
- Distinct "SCO rechazado" status when audio disconnects while opening

**Hypothesis:** The headset may be rejecting the SCO/eSCO connection due to:
1. Codec mismatch (headset prefers mSBC, we only offer CVSD with WBS disabled)
2. Missing/incorrect response to an AT command during audio setup
3. eSCO parameter negotiation failure

## What to look for in next test logs

Run with log level DEBUG to see all HFP events. Key things to check:

1. **HFP conn state=3 peer_feat=0x???**: The peer_feat bitmap shows what the
   headset supports. Bit 0x200 = Codec Negotiation (mSBC support).

2. **Audio state=1 (CONNECTING)**: Confirms esp_hf_ag_audio_connect initiated.
   If this never appears, the API call is failing.

3. **Audio state=0 (DISCONNECTED) while opening**: If this appears before
   state=2, the headset rejected the SCO. Status will show "SCO rechazado".

4. **Codec negotiated: mode=?**: If BCS event fires, shows agreed codec
   (1=CVSD, 2=mSBC).

5. **HFP evt CLCC/CNUM**: If the headset queries call state before accepting
   audio, we now respond (previously we didn't).

## Possible next steps

1. **Enable WBS (mSBC)**: Set CONFIG_BT_HFP_WBS_ENABLE=true. Modern headsets
   may require codec negotiation even if they support CVSD fallback.

2. **Check eSCO parameters**: The default CVSD eSCO S3 settings may not be
   accepted by all headsets. S4 settings or different packet types might help.

3. **Add HCI-level logging**: Enable BT debug in sdkconfig to see actual HCI
   commands and responses for Setup Synchronous Connection.

4. **Try HFP Client role**: Test with the device as HF (connecting to a phone)
   to verify SCO works in the other direction.

## Earlier findings

Disabling compiled HFP client support caused "Out of Service Records (3)";
restoring it recovered AG service connection. Only AG is initialized.
Host/controller HCI selection and a CVSD-only baseline are in place.
Unknown-codec and 3-EDR eSCO capability warnings are observations from the
Bluedroid stack, not proven causes of the timeout.

## Resume checklist

1. **Physical retest required:** Flash updated firmware, capture full DEBUG logs.
2. Analyze peer_feat to understand headset codec support.
3. If "SCO rechazado" appears, consider enabling WBS for codec negotiation.
4. Add timeout, cancellation, lost-link and tone-limit regression tests.
5. Refactor shared Bluetooth ownership before combining HID keyboard and HFP.

The host session-helper tests validate bounded buffering and cancellation, not
radio interoperability. Preserve each installation's private configuration and
recovery firmware outside this repository before any later deployment.
