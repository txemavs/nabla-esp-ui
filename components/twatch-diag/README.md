# T-Watch hardware diagnostics

The example binds its diagnostic menu to native ESPHome scripts using the
shared navigation command contract. Listing hardware does not verify it.

- Motor requests a 300 ms pulse, bounded to 1–500 ms; overlaps are ignored.
  The counter counts requests, not observed vibration.
- IR sends a short 38 kHz diagnostic burst, not an appliance command.
  The counter records requests, not reception.
- Accelerometer checks BMA423 identity at either address. Detection only;
  acceleration, steps and gestures are not yet implemented.
- Battery enables the AXP202 ADC and samples every two seconds. Missing
  battery or failed reads produce unknown (--), not zero volts.
- Side key counts and acknowledges AXP202 short-press status. It activates
  the focused menu item. Other PMU events do not activate the UI.
  Long-press power behavior is unchanged.

PEK status is polled every 100 ms independently of the IRQ line, so another
pending PMU event cannot hide a key press. Normal one-second command-state
refresh preserves focus.

Dependencies: T-Watch hardware, sensor I2C, motor script, IR transmitter and
navigation. The example owns bindings; this module owns diagnostics.
Keep installation credentials outside the public repository.

The hardware composition declares V1 pins. Confirm the physical revision before
upload; V2/V3 must not be inferred from the name alone. Existing display/touch
bring-up changes are preserved pending physical confirmation.

Firmware compilation passed with ESPHome 2026.8.2. Motor, IR and side-key physical
tests remain pending. Detection does not verify accelerometer motion.

References:
- [LilyGO V1](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/blob/master/docs/watch_2020_v1.md)
- [AXP202 registers](https://github.com/lewisxhe/AXP202X_Library/blob/master/src/axp20x.h)
