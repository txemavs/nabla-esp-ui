# Bluetooth audio handoff — 2026-09-20

Status: paused experiment, not production audio. Keep off main pending review.

## Verified evidence

Original ESP32 T-Call with compact OLED and encoder detected a Galaxy Buds Pro
headset. Authentication succeeded and HFP state 3 (SLC_CONNECTED) was logged.
The owner confirmed the connected display. This does not establish audio.

Audio opening reached the connecting state, then failed or timed out.
The owner heard no tone. No successful audio-open state, microphone input,
Assist interaction, keyboard/audio coexistence or reliable reconnect is verified.

The latest source corrects a misleading completion message emitted on opening
timeout. That correction is not yet physically tested.

## Findings

Disabling compiled HFP client support caused "Out of Service Records (3)";
restoring it recovered AG service connection. Only AG was initialized.
Host/controller HCI selection and a CVSD-only baseline did not establish sound.
Unknown-codec and 3-EDR eSCO capability warnings are observations, not proven causes.

## Resume checklist

1. Inspect the pinned ESP-IDF SCO setup, runtime data path and codec negotiation.
   Capture bounded error/status diagnostics before another hardware trial.
2. Audit legacy PCM callbacks, timer pacing, ownership, cross-task state and
   teardown. Do not record microphone content for this diagnosis.
3. Add timeout, cancellation, lost-link and tone-limit regression tests.
4. Require observed audio-open/sample flow before requesting another listening
   test. Keep the short low-level tone behind an explicit action.
5. Refactor shared Bluetooth ownership before combining HID keyboard and HFP.
6. Complete localized search/confirm/cancel/disconnect/forget menus.
7. Integrate Assist only after headset audio is functional.

The host session-helper tests validate bounded buffering and cancellation, not
radio interoperability. Preserve each installation's private configuration and
recovery firmware outside this repository before any later deployment.
