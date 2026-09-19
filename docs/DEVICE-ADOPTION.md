# Adopting a device

Nabla adapts shared application meaning to different displays and inputs.
The large touch panel and small encoder panel are both first-class targets.
A new board should contribute reusable capabilities and evidence, not a private
copy of the renderer.

Use the [catalog](DISPLAY-CATALOG.md#tested-devices) as the central status
register and the [hardware index](../hardware/README.md) for board-specific notes.

## How the project is growing

- **JC3248W535CN touch panel:** initial physical launcher and touch bring-up on
  2026-09-16 established the regular/LVGL path. Later integration evidence is
  recorded separately from the initial firmware's resource figures.
- **NodeMCU-32S with ST7735 and encoder:** deployment evidence on 2026-09-18
  extended the compact path to a physical 160×128 color display. A photo shows
  the launcher; OTA/API/MQTT checks do not establish every input/recovery case.
- **128×64 monochrome:** host fixtures exercise single-icon and text-list
  presentations. The [T-Call / SSD1309 assembly](../hardware/tcall-ssd1309.md)
  gained owner-confirmed physical operation after OTA on 2026-09-19; broader
  hardware qualification and recovery checks remain open.
- **T-Watch:** ongoing [PR #23](https://github.com/txemavs/nabla-esp-ui/pull/23)
  is a separate candidate effort. Review its current state before reusing it;
  an open PR is not a qualified target in main.

These are parallel forms of the same UI, not a ladder where the largest display
is the finished product and smaller devices are secondary.

## 1. Identify the candidate

Record the exact model/revision, MCU, measured flash/PSRAM, display controller,
native resolution, visible shape, buses, pins, power and available inputs.
Separate vendor claims from observations on the unit. A similar product name
or resolution is insufficient to reuse wiring.

Keep a working baseline and restoration procedure before flashing. Backups,
credentials, real entity names and installation addresses stay private.
Publish generic hardware facts and reusable configuration only.

## 2. Compose hardware and select presentation

Reuse or add capability packages under hardware/. A board composer combines
display, input, buses and power; a device root combines that hardware with the
regular or compact library package and its own menu.

Choose the nearest [profile](../profiles/README.md) using physical legibility,
logical dimensions and input capabilities. Decide which layer owns rotation.
Use shared spacing, fonts, focus and appearance rules. Reflow, scroll or paginate
rather than shrinking text below the profile's legibility floor.

Record whether each YAML is a complete build root, a reusable package, a host
fixture or an illustrative fragment. A fragment plus secrets is not necessarily
a compilable firmware.

## 3. Establish a host baseline

Exercise the intended logical viewport with synthetic content. Check long labels,
both locales, theme/contrast, list scrolling, initial focus, Back/Cancel and
information details. For color compact targets, check tiles as well as lists;
for 128×64, check single-icon and text-list modes.

Every action must be reachable with the actual input set. Simulated touch or
keyboard input does not prove encoder direction, debouncing or touch calibration.
Record the fixture and library revision used.

## 4. Bring up the physical unit

Start with display, backlight and navigation before adding network applications.
Check native orientation, clipped edges, text legibility, selection, activation
and Back. Verify supported rotations and corresponding touch coordinates.

Record build success separately from flash success and visible operation.
Photographs show presentation; they do not prove every interaction. Capture
resource figures with their build revision and distinguish static RAM from
runtime free heap or high-water measurements.

## 5. Add optional integrations and recovery

Add information, Wi-Fi provisioning, MQTT or Home Assistant only where needed.
Check unknown/stale/unavailable states, offline navigation, command confirmation
and cancellation. Validate the actual device's resource limits and responsiveness.

Test cold boot, reconnect, provisioning rollback and an upgrade/restore path.
A Device Builder green status proves connectivity, not UI correctness.
Long-running recovery/soak checks are separate from a successful demonstration.
Peer BLE roles and vehicle startup coordination remain planned integrations;
do not advertise them as available because a board has Bluetooth hardware.

## 6. Publish the reusable result and its limits

Add the board notes and catalog entry, with links to public packages/fixtures.
Keep menus and site bindings in the installation YAML. Pin tested revisions for
consumers; an upgrade is explicit and retains the previous pin for rollback.

Describe progress with evidence: candidate, host baseline, physical bring-up,
integration checks, and remaining qualification gates. Avoid a single
unqualified “supported” label. A target can have physical display evidence while
its Wi-Fi recovery or encoder usability is still unverified.

### Evidence record template

- Exact device/revision, display/input and chosen profile.
- Date, library commit, ESPHome version and public fixture/package path.
- Build/code-generation result; upload method and result, if attempted.
- Observed physical behavior and how it was checked.
- Resource measurement type and firmware revision.
- Known gaps, failed cases and the next validation step.
- Public photo/log excerpt only after checking for installation details/secrets.

Store detailed records in the board notes and summarize their scope in the
catalog. Update the profile/module guide if shared behavior changes. The
[roadmap](platform/ROADMAP.md) owns milestone completion; adding another working
screen does not by itself close M3.
