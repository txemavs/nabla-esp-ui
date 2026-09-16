# M3 rotation watchdog investigation

## Evidence

The first JC3248W535CN reported a previous Task WDT reset on core 1,
2026-09-16 21:15:29. The installed build was 21:02:34, public revision f4dcac8.
The matching ELF was preserved privately before any further target builds.
Decoded PC 0x420312DD is lv_obj_move_to. The stack continues through
lv_obj_refr_pos, layout_update_core, lv_obj_update_layout and the per-tile
layout call in navigation/logic.yaml at the deployed revision's line 218.
The last recorded navigation action selected the launcher rotation control.
This identifies the blocked UI layout path, not a Wi-Fi connection failure.

## Defect and correction

nav_render replaced grid descriptors (four/two columns, two/four rows),
then reassigned a single tile and synchronously requested screen-wide layout.
The remaining tiles still had old coordinates. LVGL 9.5 item_repos indexes its
calculated row/column arrays using those cell coordinates without bounds checks.
A portrait/landscape transition therefore exposed invalid intermediate grid
coordinates. This is a concrete defect consistent with the watchdog stack;
the precise path from the invalid reads to the reset is not proven on hardware.

The correction sets the launcher height and every tile's cell before one layout
pass. Font measurements then use the valid grid geometry. There are no per-tile
synchronous layout requests. Watchdog settings are unchanged.

## Verification and remaining gate

Host Spanish and English portrait builds pass; the twelve automated tests pass.
A dedicated host run exercised sixteen rotations, then advanced from rotation
focus to the first tile and opened Settings successfully. A screenshot verified
portrait geometry after rotation. This checks responsiveness and navigation;
it does not reproduce the hardware watchdog timing or certify a full M3 soak.

Before closing the issue, install an owner-approved candidate, repeat rotations
on the physical touchscreen, check crash counters/logs, and measure render latency.
Complete the power-cycle/offline/reconnect and 24-hour M3 stability gate.
No firmware was uploaded during this investigation.
