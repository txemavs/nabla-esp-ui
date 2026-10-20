# Bluetooth audio handoff — 2026-09-20

**Status: PAUSED** — experimental HFP AG bring-up, not production audio.
Keep off main pending review. HFP SLC works; SCO audio not achieved.

## What works

- Device discovery with bounded candidate list
- Pairing and authentication (SSP numeric comparison)
- HFP SLC (Service Level Connection) reaches state 3 (SLC_CONNECTED)
- HA integration: Scan/Select/Confirm/Connect/Disconnect/Test Tone buttons
- Status strings: "HFP conectado (sin audio)", "SCO rechazado", etc.
- Diagnostic logging: HFP events, peer_feat, BCS codec negotiation, audio states

## What fails

- SCO/eSCO audio connection: never reaches CONNECTED (state 2) or CONNECTED_MSBC (state 3)
- No audible test tone played
- After SCO rejection, SLC sometimes drops within seconds

## Hardware under test

- **Kit1:** NodeMCU-32S (original ESP32, not S3) + Galaxy Buds Pro
- **Earlier:** T-Call + external OLED showed same SLC-ok / audio-fail pattern

## Investigation history (2026-09-20)

| Fix | Commit | Change | Result |
|-----|--------|--------|--------|
| #1 | 1422bbf | MAX_SYNC_CONN=1 (was 0) | 10s timeout, no audio |
| #2 | 99a7c0a | DEBUG diagnostics, CLCC/CNUM handlers | Timeout then HFP drop |
| #3 | eb43515 | WBS_ENABLE=true for codec negotiation | "SCO rechazado" ~3s (faster rejection) |
| #4 | 2899d28 | Simulate outgoing call before audio_connect | **Still "SCO rechazado" at 19:32 CEST** |

All four fixes attempted on Kit1 + Galaxy Buds. None achieved SCO CONNECTED.
The progression from 10s timeout → 3s active rejection confirms diagnostics
are working and the headset is actively refusing the SCO link.

## Not in scope / known limits

- **ESP32-S3:** Has no classic Bluetooth (BR/EDR), only BLE. HFP requires classic.
- **ESPHome:** Has no stock HFP AG component; this is custom external_component.
- **A2DP:** Separate profile for streaming audio; not attempted here.
- **Assist/satellite:** Requires working SCO bidirectional audio first.

## Current state

- Bringup firmware `kit1-hfp-bringup` may still be on Kit1
- Production UI YAML in repo is untouched (no HFP integration)
- Branch `codex/bluetooth-audio-bringup` / PR #28 contains all experimental code

## Resume checklist

When work resumes:

1. **Capture DEBUG logs** with peer_feat bitmap, BCS events, and audio state
   transitions (1 CONNECTING → 0 DISCONNECTED sequence).

2. **Try different headset** — a simpler BT 4.x headset may help isolate
   whether this is Galaxy Buds-specific behavior.

3. **Run ESP-IDF HFP AG example side-by-side** on same hardware to verify
   ESP32 can do SCO at all with this headset.

4. **Check eSCO parameters** — default S3/S4 settings may not be accepted;
   some headsets need specific packet types.

5. **Enable HCI-level logging** (CONFIG_BT_BLUEDROID_DEBUG=y) to see actual
   Setup Synchronous Connection HCI commands and error codes.

6. **Try active call state** — instead of OUTGOING_ALERTING, try
   CALL_IN_PROGRESS with CALL_SETUP_IDLE.

Do not keep flashing blind sdkconfig toggles. Each test should add diagnostic
evidence about why SCO fails.

## Earlier findings

- Disabling compiled HFP client support caused "Out of Service Records (3)";
  restoring it recovered AG service connection. Only AG is initialized.
- Unknown-codec and 3-EDR eSCO capability warnings are observations from the
  Bluedroid stack, not proven root causes.

## Notes

The host session-helper tests validate bounded buffering and cancellation, not
radio interoperability. Preserve each installation's private configuration and
recovery firmware outside this repository before any later deployment.
