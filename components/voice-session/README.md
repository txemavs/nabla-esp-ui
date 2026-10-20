# Voice session foundation

Shared preparation for T-Watch and T-Call as voice terminals for an assistant
running on Linux. This is a tested C++ session helper, not a working voice client.

The intended path is microphone/headset -> ESP32 -> authenticated Wi-Fi link ->
Linux assistant, with the spoken answer returning through the same terminal.
Bluetooth headsets require an HFP Audio Gateway adapter on the original ESP32.
Local audio is an independent adapter; no unverified microphone/speaker pins
are supplied. Linux service/protocol selection is still required.

Implemented: explicit start, half-duplex listening/waiting/speaking states,
microphone mute, cancellation on transport loss, and a fixed 6,400-byte PCM
buffer. The internal format is mono signed PCM16 at 16 kHz; adapters must convert
negotiated HFP rates rather than treating 8 kHz audio as 16 kHz.
Buffer limits reject a whole write instead of silently truncating it. The owner
must handle overflow/underrun and timeouts. Drain capture before finish_capture;
drain playback before finish_response. Cancel zeros buffered audio.

One task owns this helper. Radio callbacks must enqueue events separately.
Mute cancels the current session. Starting is explicit: pairing alone never
starts microphone capture. The helper performs no network I/O and logs nothing.

Export voice_session.h using nabla_runtime headers:
components/voice-session/voice_session.h. No ESPHome voice component schema
or Settings menu is exposed yet.

Pending: shared Bluetooth lifecycle with HID, functional HFP audio, codec/rate conversion,
authenticated Linux transport, source/sink binding, adaptive UI, and hardware
verification. The future UI must retain Back/Cancel and an active-microphone
indicator on both compact and regular profiles.
See [delivery gates](../../docs/platform/AUDIO.md).
