# Reusable audio module: implementation contract

Status: experimental. Discovery, pairing and HFP service connection have been
observed; audio playback and Assist remain unverified.
See [handoff](AUDIO-STATUS.md) for findings and next steps.

## Scope and boundaries

Support the T-Watch 2020 and T-Call as consumers of the same optional module.
Device YAML owns verified audio wiring and menu placement; shared code owns
session state, bounded buffering and actions. Home Assistant is not required.
Do not infer microphone or speaker presence from the enclosure or board name.

Separate three responsibilities:

- A shared classic Bluetooth service owns controller/Bluedroid startup, GAP
  callback dispatch and pairing. HID and HFP register with it rather than each
  initializing the radio or replacing the other's callback.
- An HFP adapter owns service/audio connection state and the selected role.
  Audio Gateway connects to headsets; Hands-Free Client connects to a phone.
  The selected initial role is Audio Gateway for an external headset; Linux runs the voice assistant. Do not imply both can run together.
- Local audio adapters own microphone/speaker hardware using verified ESPHome
  interfaces where possible. Bluetooth headsets provide their own audio endpoints;
  no local microphone or speaker is required for that route.

Receiving headset audio still needs an application consumer, and sending audio
needs an application source. HFP alone does not provide telephony, speech
recognition or a Raspberry Pi audio bridge.

## Target constraints

Original ESP32 supports classic HFP. ESP32-S3 has BLE but not classic HFP;
ESP32-S2 has no integrated Bluetooth. Reject unsupported configurations early.
The T-Watch public profile targets V1 wiring; verify the actual revision before
adding I2S/PDM pins or enabling a power rail. The T-Call SSD1309 profile uses
Arduino and reserves display/encoder pins; evaluate an ESP-IDF composition
without changing the working hardware package in place. Its disabled modem's
analog audio connections are not an ESP32 audio codec.

## Shared UI and state

Settings > Connections > Bluetooth handles explicit peer selection, pairing
with a visible PIN, Disconnect and Forget. Settings > Audio exposes only
available routes, volume, output mute and microphone mute.
Use existing shared list/form presentation on 128x64 and 240x240 displays,
including encoder-accessible Back/Cancel and the established logo.

Distinguish unavailable hardware, disconnected, pairing, service connected,
audio opening, audio active and failure. A paired or service-connected device
must not be displayed as carrying live audio. Do not record microphone samples,
keys, PINs or bond material in logs. Store bonds locally, outside public YAML.
Microphone capture requires an explicit start and visible active state.

## Delivery gates

1. Extract shared Bluetooth lifecycle and GAP dispatch from the keyboard;
   retain bounded callback queues and verify keyboard pairing/navigation.
2. Implement one selected HFP role with ESP-IDF's supported APIs; validate schema
   and build an isolated generic fixture before integration into either device.
3. Add bounded mono PCM buffers with negotiated sample rate, underrun silence,
   overflow accounting and no blocking I/O or allocation in audio callbacks.
   Never update LVGL or compact display objects from Bluetooth callbacks.
4. Bind an explicit audio source/sink and shared UI actions. A finite test tone
   is a separate action, not an automatic side effect of connecting a headset.
5. Qualify T-Watch and then T-Call: audio both directions, mute, disconnect,
   reconnect, radio loss, simultaneous keyboard, Wi-Fi and responsive menus.
   Measure free/minimum heap, buffer loss and latency; check OTA recovery.
6. Publish reusable code and measured evidence. Keep private peers, credentials,
   network configuration and firmware outside the repository.

No device should be flashed merely to mark the module as prepared.

## References

- [HFP Audio Gateway](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/esp_hf_ag.html)
- [HFP Hands-Free Client](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/bluetooth/esp_hf_client.html)
- [Keyboard adapter](../../external_components/nabla_bt_keyboard/README.md)
- [T-Call hardware](../../hardware/tcall-ssd1309.md)
- [Shared UI contract](../UI-CONSISTENCY.md)
