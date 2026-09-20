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

### Fix #2: Enhanced diagnostics and missing AT handlers (commit 99a7c0a)

Added comprehensive logging and missing HFP AT command handlers:
- All HFP callback events logged with names
- Connection state logs peer_feat and chld_feat (shows headset capabilities)
- BCS codec negotiation event logged
- CLCC (call list) and CNUM (subscriber number) responses added
- Audio state transitions logged with state names
- Distinct "SCO rechazado" status when audio disconnects while opening

**Result:** Physical retest still showed timeout then HFP drop — consistent
with headset rejecting SCO.

### Fix #3: Enable WBS for codec negotiation (current)

Enabled CONFIG_BT_HFP_WBS_ENABLE=true in sdkconfig. Modern headsets like
Galaxy Buds Pro support HFP 1.7+ codec negotiation and may require the AG to
advertise mSBC capability even if CVSD fallback is acceptable. With WBS
disabled, the AG's SDP record doesn't include codec negotiation, so the
headset may reject the SCO setup.

Changes:
- CONFIG_BT_HFP_WBS_ENABLE=true (was false)
- Added WBS event logging (ESP_HF_WBS_RESPONSE_EVT)
- Kept all diagnostics from Fix #2

**Hypothesis:** Galaxy Buds Pro sees the AG doesn't support codec negotiation
and rejects the SCO because it can't confirm CVSD compatibility without the
negotiation exchange. Enabling WBS lets the AG participate in the BCS
(Bluetooth Codec Selection) procedure where both sides agree on CVSD or mSBC.

## What to look for in next test logs

Run with log level DEBUG to see all HFP events. Key things to check:

1. **HFP conn state=3 peer_feat=0x???**: The peer_feat bitmap shows what the
   headset supports. Bit 0x200 = Codec Negotiation (mSBC support).

2. **WBS current codec: mode=?**: With WBS enabled, this event may fire during
   connection setup showing the initial codec mode.

3. **Codec negotiated: mode=?**: BCS event shows agreed codec (1=CVSD, 2=mSBC).
   If this appears, codec negotiation succeeded.

4. **Audio state=1 (CONNECTING)**: Confirms esp_hf_ag_audio_connect initiated.

5. **Audio state=2 (CONNECTED) or =3 (CONNECTED_MSBC)**: Success! Tone should
   play. State 2 means CVSD, state 3 means mSBC.

6. **Audio state=0 (DISCONNECTED) while opening**: Headset still rejecting.
   Status shows "SCO rechazado".

## If WBS doesn't work: further steps

1. **Check eSCO parameters**: The default eSCO S3/S4 settings may not be
   accepted. Some headsets need specific packet types or intervals.

2. **Add HCI-level logging**: Enable CONFIG_BT_BLUEDROID_DEBUG=y to see actual
   HCI commands and responses for Setup Synchronous Connection.

3. **Try different headset**: Test with a simpler BT 4.x headset to rule out
   Galaxy Buds-specific behavior.

## Earlier findings

Disabling compiled HFP client support caused "Out of Service Records (3)";
restoring it recovered AG service connection. Only AG is initialized.
Host/controller HCI selection is in place.
Unknown-codec and 3-EDR eSCO capability warnings are observations from the
Bluedroid stack, not proven causes of the timeout.

## Resume checklist

1. **Physical retest required:** Owner will flash updated firmware on Kit1.
2. Capture full DEBUG logs if audio still fails.
3. If still rejected, enable HCI-level debug to see actual eSCO setup commands.
4. Add timeout, cancellation, lost-link and tone-limit regression tests.
5. Refactor shared Bluetooth ownership before combining HID keyboard and HFP.

The host session-helper tests validate bounded buffering and cancellation, not
radio interoperability. Preserve each installation's private configuration and
recovery firmware outside this repository before any later deployment.
